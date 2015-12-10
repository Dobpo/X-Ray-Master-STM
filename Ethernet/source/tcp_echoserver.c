/**
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * LwIP - Lightweight Internet Protocol - TCP/IP стек с открытым кодом.
 * pbuf - Packet buffer - структура данных для управления буффером пакетов.
 * pcb - protocol control block - блок управления протоколом.
 * argument - аргументы, параметры.
 **/
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "stm32f4xx.h"
#include "main.h"
#include "tcp_echoserver.h"

#if LWIP_TCP

//Структура TCP pcb.
static struct tcp_pcb *tcp_echoserver_pcb;

/* Состояния ECHO protocol */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* структура для содержания информации о соединение что передается как аргумент
   для LwIP callbacks(функциq обратного вызова)*/
struct tcp_echoserver_struct
{
  u8_t state;             /* текущее состояние соединения */
  struct tcp_pcb *pcb;    /* указатель на текущий tcp_pcb */
  struct pbuf *p;         /* указатель на полученый/должен будет передан pbuf */
};

//PCB для использования в прерывании**************
//__IO struct tcp_pcb *Current_PCB;

//Функции обратного вызова tcp сервера
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);

/**
  * @brief  Инициализация tcp сервера
  * @param  None
  * @retval None
  */
void tcp_echoserver_init(void)
{
  /* создает новый pcb(блок управления протоколом) */
  tcp_echoserver_pcb = tcp_new();

  if (tcp_echoserver_pcb != NULL)
  {
    err_t err;
    
    /* назначение текущему echo_pcb заданый IP адрес и порт*/
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, 9670);
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb
       * начинает процесс прослушивания для echo_pcb*/
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
      
      /* initialize LwIP tcp_accept callback function
       * инициализация LwIP функций обратного вызова
       * назначает функцию обратного вызова которая вызовется когда будет установлено
       * новое tcp соединение*/
      tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);
      //Помещает данные в очерень на отправление, но не отсылает.
      //Сдесь возможно какието служебные данные
      tcp_write(tcp_echoserver_pcb, 1, 1, TCP_WRITE_FLAG_MORE);
    }
    else 
    {
      /* deallocate the pcb
       * освобождает pcb*/
      memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
      //printf("Can not bind pcb\n");
    }
  }
  else
  {
    //printf("Can not create new pcb\n");
  }
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @Краткое описание: Эта функция будет вызвана при появлении нового tcp соединения
  * 				   для назначения функций обр. вызова.
  * @param  arg: not used
  * @param  newpcb: указатель на tcp_pcb структуру для вновь созданого tcp соединения
  * @param  err: not used 
  * @retval err_t: error status
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority accepted tcp connection newpcb
   * установить приоритет для вновь принятого tcp соединение newpcb*/
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* allocate structure es to maintain tcp connection informations
   * распределите структуру es, чтобы поддерживать информацию tcp связи*/
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    
    /* pass newly allocated es structure as argument to newpcb
     * назначает параметры которые должны быть переданы в функцию обр. вызова*/
    //Параметром для обр. функц. является tcp_echoserver_struct(es) которая содержит
    //status , pcb и pbuf.
    tcp_arg(newpcb, es);
    
    /* initialize lwip tcp_recv callback function for newpcb
     * назначает функию обр. выз. что будет вызвана при поступлении новых данных */
    tcp_recv(newpcb, tcp_echoserver_recv);
    
    /* initialize lwip tcp_err callback function for newpcb
     * назначает функию обр. выз. что будет вызвана при возникновении ошибки
     * (например нехватка памяти)*/
    tcp_err(newpcb, tcp_echoserver_error);
    
    /* initialize lwip tcp_poll callback function for newpcb
     * назначает функию обр. выз., которая вызывается 2 раза в сек
     * может быть использована проверки наличия данных для отправки или
     * наличия соединения которое нужно закрыть*/
    tcp_poll(newpcb, tcp_echoserver_poll, 1);
    
    //@todo: Получаем текущий tcp_pcb для использования в tcp_write()
    Current_PCB = newpcb;

    ret_err = ERR_OK;
  }
  else
  {
    /*  close tcp connection
     * закрыть tcp соединение*/
    tcp_echoserver_connection_close(newpcb, es);
    /* return memory error
     * возвращает ошибку памяти*/
    ret_err = ERR_MEM;
  }
  return ret_err;  
}


/**
  * @brief This function is the implementation for tcp_recv LwIP callback
  * @Краткое описание:  Эта обр. функция вызывается когда приходят новые данные.
  * @param  arg: указатель на аргумент(параметры) для tcp_pcb соединения
  * @param  tpcb: указатель на tcp_pcb connection
  * @param  pbuf: указатель на received pbuf
  * @param  err: информация об ошибки относительно полученного pbuf
  * @retval err_t: код ошибки
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	//@todo: First recvest for in data
	struct tcp_echoserver_struct *es;
	err_t ret_err;

	es = (struct tcp_echoserver_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection
   * если получили пустой пакет от клиента => закрываем соединение*/
  if (p == NULL)
  {
    /* remote host closed connection
     * удаленный клиент закрыл соединение*/
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection
        * мы сделали посылку закрываем соединение*/
       tcp_echoserver_connection_close(tpcb, es);
    }
    else
    {
      /* we're not done yet
       * мы еще не готовы*/
      /* acknowledge received packet
       * подтвердить полученые пакета*/
      tcp_sent(tpcb, tcp_echoserver_sent);
      
      /* send remaining data
       * послать оставшиеся данные*/
      tcp_echoserver_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    es->p = NULL;
    pbuf_free(p);
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
	//@todo:
	/** В этом месте происходил возврат эхо данных первый раз**************************/
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
        /* store reference to incoming pbuf (chain) */
    es->p = p;
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(tpcb, tcp_echoserver_sent);
    
    /* send back the received data (echo)
     * отправляет обратно полученные данные*/
    tcp_echoserver_send(tpcb, es);

    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {
	/* @todo: Another recvest for in data
     * отправка данных второй раз и далее
     * more data received from client and previous data has been already sent
     * были получены еще данные от клиента и предведущие данные были отосланы*/
    if(es->p == NULL)
    {

    	//}
      es->p = p;

      /* send back received data
       * отослать обратно полученные данные*/
      tcp_echoserver_send(tpcb, es);

    }
    else
    {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously
       * привязывает pbufs к концу того что мы приналя ранее*/
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }
  
  /* data received when connection already closed
   * данные получены когда соединение уже закрыто*/
  else
  {
    /* Acknowledge data reception
     * подтверждает прием данных*/
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing
     * освободить pbuf и ничего не делать*/
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs. 
  * @Краткое описание: Эта обр. функция вызывается при возникновении фатальной
  * 				   ошибки tcp соединения
  * @param  arg: указатель на argument parameter
  * @param  err: не исрользуется
  * @retval Нет
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure
     * освобождение es структуры*/
    mem_free(es);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @Краткое описание: Данная функция вызывается периодически. Может быть использована
  * 				   в приложении для проверки наличия данных которые должны быть отправлены
  * 				   или наличия соединения которое должно быть закрыто.
  * @param  arg: указатель на argument passed to callback
  * @param  tpcb: указатель на tcp_pcb для актуального tcp соединения
  * @retval err_t: error code
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data
       * оставшиеся pbuf(цепь), попытатся отослать данные*/
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)
       * нет оставшихся pbuf (цепь)*/
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection
         * закрыть tcp соединение*/
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done
     * нет ничего  для выполнения*/
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @Краткое описание: Эта обр. функция  вызывается когда отосланые данные подтверждены
  * 				   удаленным хостом.
  * @param  Нет
  * @retval Нет
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send
     * еще есть pbufs для отсылки*/
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection
     * если данные закончились и клиент закрыл соединение*/
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}


/**
  * @brief  This function is used to send data for tcp connection
  * @Краткое описание: Эта функция используется для отправки данных для tcp соединения
  * @param  tpcb: указатель на the tcp_pcb connection
  * @param  es: указательн на echo_state structure
  * @retval None
  */
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure
     * получить указатель на pbuf с структуры es*/
    ptr = es->p;

    /* enqueue data for transmission
     * поставьте в очередь данные для передачи*/
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK)
    {
      u16_t plen;

      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any)
       * продолжает со следующим pbuf в цепи (если имеется)*/
      es->p = ptr->next;
      
      if(es->p != NULL)
      {
        /* increment reference count for es->p
         * увеличивает  счетчик для es->p*/
        pbuf_ref(es->p);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);

      /* Update tcp window size to be advertized : should be called when received
      data (with the amount plen) has been processed by the application layer */
      tcp_recved(tpcb, plen);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later / harder, defer to poll */
     es->p = ptr;
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * @brief  Эта функция закрывает tcp соединение
  * @param  tcp_pcb: указатель на tcp соединение
  * @param  es: указатель на echo_state structure
  * @retval None
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  
  /* remove all callbacks
   * удаление всех функций обратного вызова*/
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* удаляет es structure */
  if (es != NULL)
  {
    mem_free(es);
  }  
  
  /* close tcp connection
   * закрыть tcp соединение */
  tcp_close(tpcb);
}

#endif
