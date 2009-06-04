#ifndef _SEMAPHORE_INC
#define _SEMAPHORE_INC


#define SEMAPH_NORMAL   1
#define SEMAPH_INVERSE  2

#define SEMAPH_NOWAIT         0  
#define SEMAPH_WAITTIME    1000      //  1 sec

#define SEMAPH_TIMEOUT   120000L     //  2 min
#define SEMAPH_BLOCK         -1L


/*
 Possibilidades de Inicializacao
 
 SEMAPH_NORMAL -> _SEMAPHORE(SEMAPH_NORMAL, int nMaxRes)
 Ao inicializar deve-se informar o numero maximo de recursos disponiveis a serem compartilhados
 A funcao Wait() retorna true se for possivel alocar recurso e decrementa contador
 Caso contrario (nRes=0) retorna false
 A funcao Signal() indica a liberacao do recurso por parte da aplicacao e incrementa contador
 de recursos disponiveis
 
 SEMAPH_INVERSE -> _SEMAPHORE(SEMAPH_INVERSE)
 Neste caso o semaforo funciona como um contador de recursos pendentes
 a serem processados antes de liberar as tarefas concorrentes
 O semaforo eh inicializado com contador de recursos=0
 A funcao AddPend() incrementa o numero  de recursos pendentes
 A funcao Signal() decrementa o numero de recursos pendentes
 A funcao Wait() retorna true caso o contador de recursos (pendencias) = 0 
 Caso contrario retorna false
*/

class _SEMAPHORE
{
   private:

      int   SType;
      UINT  nRes;

      DWORD dwWaitCycleTime;
      DWORD dwTimeout;

      CRITICAL_SECTION	m_cs;

   public:

      _SEMAPHORE(int _SType=SEMAPH_NORMAL, int _nMaxRes=1, DWORD _WaitTime=SEMAPH_NOWAIT, DWORD _timeout=SEMAPH_BLOCK);
     ~_SEMAPHORE(void);

      void SetWaitTime(DWORD _time)   { dwWaitCycleTime = _time; }
      void SetTimeout (DWORD _time)   { dwTimeout       = _time; }

      void Signal   (void);
      void AddPend  (void);
      bool Wait     (bool flWait=true);
};



#endif // _SEMAPHORE_INC

