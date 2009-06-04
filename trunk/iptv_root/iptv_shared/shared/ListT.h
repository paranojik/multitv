#ifndef _ListT_STR
#define _ListT_STR


#include "compat.h"


template <class T> struct Node
{
   char nome[TNOME_OBJ+1];

   T       *pobj;   // Ptr p/ ObjBase

   Node<T> *pPrev,  // Nodo Anterior da Lista
           *pNext;  // Proximo Nodo da Lista

   ULONG64 val;     // Para o caso de Priority Lists


   Node(void)
   {
      nome[0]  = '\0';
      pobj     = NULL;
      pPrev    = NULL;
      pNext    = NULL;
      val      = 0;
   }

   void SetName(LPCSTR _pnome=NULL)
   {
      if (_pnome)
         ajusta_upp_str(_pnome, nome, TNOME_OBJ);
      else
         nome[0]  = '\0';
   }
};


/*

Class definition

template <class T> class ListT
{
   private:

      Node<T>  *pFirst,    // Ptr p/ prim. elem. da Lista (Node)
               *pLast;     // Ptr p/ ult. elem. da Lista  (Node)

      Node<T>  *pPos;      // Ptr p/ posicao do ptr de consulta

      bool   flKillRegs;   // Indica se os objetos serao destruidos no destrutor da lista

      Node<T> * CreateNode(T *pobj, ULONG64 val=0, LPCSTR nome=NULL);
      void   kill      (void);


   public:

      ListT          (bool _flKill=true);
     ~ListT          (void);

      long get_nitens(void);

      void reset           (void);
      Node<T> * append     (T *pobj, LPCSTR nome=NULL, ULONG64 val=0, Node<T> *_pNode=NULL);
      Node<T> * insertTop  (T *pobj, LPCSTR nome=NULL, ULONG64 val=0);
      Node<T> * push       (T *pobj, LPCSTR nome=NULL, ULONG64 val=0, Node *_pNode=NULL);
      T       * pop        (bool _flKeepObj=true);
      bool      update     (Node<T> *pNode, ULONG64 _newval=0);
      Node<T> * GetCurNode (void);

      T * search(long ind, Node<T> **pNodeFound=NULL)    // Indice 1..nItens
      Node<T> * search     (T *pobj);
      T * search           (LPCSTR nome, Node<T> **pNodeRet=NULL);
      bool test            (LPCSTR nome);
      bool SetName         (T *pobj, LPCSTR nome);

      T * first      (LPCSTR *pName=NULL);
      T * last       (LPCSTR *pName=NULL);
      T * next       (LPCSTR *pName=NULL);
      T * prev       (LPCSTR *pName=NULL);
      T * current    (LPCSTR *pName=NULL);

      void SetPos    (Node<T> *pNewPos);

      bool erase     (long ind, bool flKeepObj=false);
      bool erase     (Node<T> *pNode, bool flKeepObj=false, bool flKeepNode=false);
      bool erase     (LPCSTR nome, bool flKeepObj=false);
      bool erase     (T *pobj, bool flKeepObj=false);
      bool eraseCur  (bool flKeepObj=false);
};

*/


template <class T> class ListT
{
   private:

      Node<T>  *pFirst, // Ptr p/ prim. elem. da Lista (Node)
               *pLast;  // Ptr p/ ult. elem. da Lista  (Node)

      Node<T>  *pPos;   // Ptr p/ posicao do ptr de consulta

      bool   flKillRegs,// Indica se os objetos serao destruidos no destrutor da lista
             flAscend,  // Para o caso de Priority Lists indica se a ordenacao
                        // sera crescente ou decrescente
             flSkipNext;// Guarda ultimo comando de skip

      long   nItens;    // Numero de Itens contidos na lista


      Node<T> * CreateNode(T *pobj, ULONG64 val=0, LPCSTR nome=NULL)
      {
         Node<T> *pNode; 

         pNode = new Node<T>;

         if (pNode)
         {
            nItens++;

            pNode->pobj = pobj;
            pNode->val  = val;

            pNode->SetName(nome);
         }

         return pNode;
      }


      void kill(void)
      {
         Node<T>  *pNode,
                  *pNext;

         T     *pobj;


         pNode = pFirst;

         while (pNode)
         {
            pNext  = pNode->pNext;
            pobj   = pNode->pobj;

            delete pNode;

            if (pobj && flKillRegs)
               delete pobj;

            pNode = pNext;
         }

         nItens = 0;
      }

   
   public:

      ListT(bool _flKill=true, bool _flAscend=true)
      {
         pFirst      = NULL;
         pLast       = NULL;
         pPos        = NULL;

         flKillRegs  = _flKill;
         flAscend    = _flAscend;
         flSkipNext  = true;

         nItens      = 0;
      }


     ~ListT(void)
      {
         kill();
      }


      void reset(void)
      {
         kill();

         pFirst      = NULL;
         pLast       = NULL;
         pPos        = NULL;
      }


      long get_nitens(void)
      {
         return nItens;
      }


      Node<T> * append(T *pobj, LPCSTR nome=NULL, ULONG64 val=0, Node<T> *_pNode=NULL)
      {
         Node<T> *pNode; 


         if (_pNode)
            pNode = _pNode;
         else
            pNode = CreateNode(pobj, val, nome);

         if (pNode)
         {
            if (!pFirst)
               pFirst = pNode;
            else
            {
               pNode->pNext = NULL;
               pNode->pPrev = pLast;
               pLast->pNext = pNode;
            }

            pLast = pNode;
         }

         return pNode;
      }


      // Insere nodo na 1a posicao da lista

      Node<T> * insertTop(T *pobj, LPCSTR nome=NULL, ULONG64 val=0)
      {
         Node<T> *pNode; 


         pNode = CreateNode(pobj, val, nome);

         if (pNode)
         {
            if (!pFirst)
            {
               pFirst = pNode;
               pLast  = pNode;
            }
            else
            {
               pFirst->pPrev = pNode;
               pNode->pNext  = pFirst;

               pFirst = pNode;
            }

         }

         return pNode;
      }


      bool update(Node<T> *pNode, ULONG64 _newval=0)
      {
         bool ret;

         ret = false;

         if (pNode)
         {
             if ( erase(pNode, true, true) )
             {
                pNode->val = _newval;

                if ( push(pNode->pobj, pNode->nome, pNode->val, pNode) )
                   ret = true;
             }
         }

         return ret;
      }


      // Insere nodo na lista ordenado pelo valor (Priority List)

      Node<T> * push(T *pobj, LPCSTR nome=NULL, ULONG64 val=0, Node<T> *_pNode=NULL)
      {
         Node<T> *pNode,
                 *pLocation,
                 *pPrev,
                 *pNext;


         // Verifica posicao onde o Nodo sera inserido

         if (flAscend)
         {
            if (!pLast || pLast->val<=val)
               pLocation = NULL;
            else
            {
               pLocation = pFirst;

               while (pLocation && pLocation->val<=val)
                  pLocation = pLocation->pNext;
            }
         }
         else
         {
            if (!pLast || pLast->val>=val)
               pLocation = NULL;
            else
            {
               pLocation = pFirst;

               while (pLocation && pLocation->val>=val)
                  pLocation = pLocation->pNext;
            }
         }

         if (pLocation)
         {
            if (_pNode)
               pNode = _pNode;
            else
               pNode = CreateNode(pobj, val, nome);

            if (pNode)
            {
               pPrev = pLocation->pPrev; 
               pNext = pLocation->pNext;

               if (pPrev)
               {
                  pNode->pPrev   = pPrev;
                  pPrev->pNext   = pNode;
               }
               else
               {
                  pNode->pPrev   = NULL;
                  pFirst         = pNode;
               }

               pNode->pNext      = pLocation;
               pLocation->pPrev  = pNode;
            }
         }
         else
            pNode = append(pobj, nome, val, _pNode);

         return pNode;
      }


      // Retorna o 1o nodo da lista de acordo com a ordenacao
      // informada no construtor da classe
      // O nodo eh removido da lista em seguida, porem o objeto
      // contido no mesmo nao eh apagado

      T * pop(bool _flKeepObj=true)
      {
         T *pobj;

         pobj = first();

         if (pobj)
            eraseCur(_flKeepObj);

         return pobj;
      }


      T * search(long ind, Node<T> **pNodeFound=NULL)
      {
         long i;
         Node<T> *pNode;
         T *pobj;

         pobj = NULL;

         if (pNodeFound)
            *pNodeFound = NULL;

         if (ind>0 && ind<=nItens)
         {
            i     = 1;
            pNode = pFirst;

            while (pNode && i<ind)
            {
               pNode = pNode->pNext;
               i++;
            }

            if (pNode)
            {
               pobj = pNode->pobj;

               if (pNodeFound)
                  *pNodeFound = pNode;
            }
         }

         return pobj;
      }


      Node<T> * search(T *pobj)
      {
         Node<T> *pNode;


         pNode = pFirst;

         while (pNode)
         {
            if ( pNode->pobj == pobj )
               break;
            else
               pNode = pNode->pNext;
         }

         return pNode;
      }


      T * search(LPCSTR nome, Node<T> **pNodeRet=NULL)
      {
         T    *pobj;
         bool  ret;

         Node<T> *pNode;


         ret   = false;
         pNode = pFirst;

         if (nome)
         {
            while (pNode && !ret)
            {
               if ( (_fstricmp(nome, pNode->nome) == 0) )
                  ret   = true;
               else
                  pNode = pNode->pNext;
            }
         }

         if (pNodeRet)
            *pNodeRet = (ret ? pNode:NULL);

         if (ret)
            pobj = pNode->pobj;
         else
            pobj = NULL;

         return pobj;
      }


      bool test(LPCSTR nome)
      {
         bool ret;
         Node<T> *pNode;

         search(nome, &pNode);

         ret = (pNode ? true:false);

         return ret;
      }


      bool SetName(T *pobj, LPCSTR nome)
      {
         bool ret;
         Node<T> *pNode;

         ret = false;

         if ( pobj &&& (pNode=search(pobj))!=NULL )
         {
            ret = true;

            pNode->SetName(nome);
         }

         return ret;
      }


      T * first(LPCSTR *pName=NULL)
      {
         T *pobj;


         flSkipNext  = true;
         pPos        = pFirst;

         if (pPos)
         {
            pobj = pPos->pobj;

            if (pName)
               *pName = pPos->nome;
         }
         else
         {
            pobj = NULL;

            if (pName)
               *pName = NULL;
         }

         return pobj;
      }


      T * last(LPCSTR *pName=NULL)
      {
         T *pobj;


         flSkipNext  = false;
         pPos        = pLast;

         if (pPos)
         {
            pobj = pPos->pobj;

            if (pName)
               *pName = pPos->nome;
         }
         else
         {
            pobj = NULL;

            if (pName)
               *pName = NULL;
         }

         return pobj;
      }


      T * next(LPCSTR *pName=NULL)
      {
         T *pobj;


         flSkipNext  = true;

         if (pPos)
         {
            if (pPos == (Node<T> *) (-1))
               pPos = pFirst;
            else
               pPos = pPos->pNext;
         }

         if (pPos)
         {
            pobj = pPos->pobj;

            if (pName)
               *pName = pPos->nome;
         }
         else
         {
            pobj = NULL;

            if (pName)
               *pName = NULL;
         }

         return pobj;
      }


      T * prev(LPCSTR *pName=NULL)
      {
         T *pobj;


         flSkipNext  = false;

         if (pPos)
         {
            if (pPos == (Node<T> *) (-1))
               pPos = pLast;
            else
               pPos = pPos->pPrev;
         }

         if (pPos)
         {
            pobj = pPos->pobj;

            if (pName)
               *pName = pPos->nome;
         }
         else
         {
            pobj = NULL;

            if (pName)
               *pName = NULL;
         }

         return pobj;
      }


      Node<T> * GetCurNode(void)
      {
         Node<T> *pNode;


         if (pPos && pPos != (Node<T> *) (-1))
            pNode = pPos;
         else
            pNode = NULL;

         return pNode;
      }


      T * current(LPCSTR *pName=NULL)
      {
         T *pobj;
         Node<T> *pNode;

         pNode = GetCurNode();

         if (pNode)
         {
            pobj = pNode->pobj;

            if (pName)
               *pName = pNode->nome;
         }
         else
         {
            pobj = NULL;

            if (pName)
               *pName = NULL;
         }

         return pobj;
      }


      void SetPos(Node<T> *pNewPos)
      {
         pPos = pNewPos;
      }


      bool erase(Node<T> *pNode, bool flKeepObj=false, bool flKeepNode=false)
      {
         bool ret;

         Node<T> *pNext,
                 *pPrev;

         if (pNode)
         {
            ret = true;

            pNext = pNode->pNext;
            pPrev = pNode->pPrev;

            if (!flKeepObj && flKillRegs && pNode->pobj)
               delete pNode->pobj;

            if (pNode == pPos)
            {
               // Ajusta ponteiro para proximo prev() ou next()

               if (flSkipNext)
                  pPos = pNode->pPrev;
               else
                  pPos = pNode->pNext;

               // Informa que o proximo next deverah
               // ajustar o ponteiro devidamente retornando
               // pFirst ou pLast

               if (!pPos)
                  pPos = (Node<T> *) (-1);
            }

            if (pPrev)
               pPrev->pNext = pNext;
            else
               pFirst       = pNext;

            if (pNext)
               pNext->pPrev = pPrev; 
            else
               pLast        = pPrev;

            if (!flKeepNode)
            {
               delete pNode;
               nItens--;
            }
         }
         else
            ret = false;

         return ret;
      }


      bool erase(long ind, bool flKeepObj=false)
      {
         bool  ret;
         Node<T> *pNode;

         if ( search(ind, &pNode) && erase(pNode, flKeepObj) )
            ret = true;
         else
            ret = false;

         return ret;
      }


      bool eraseCur(bool flKeepObj=false)
      {
         bool ret;
         Node<T> *pNode;

         pNode = GetCurNode();

         if (pNode && erase(pNode, flKeepObj))
            ret = true;
         else
            ret = false;

         return ret;
      }


      bool erase(LPCSTR nome, bool flKeepObj=false)
      {
         bool  ret;
         Node<T> *pNode;

         search(nome, &pNode);

         if ( pNode && erase(pNode, flKeepObj) )
            ret = true;
         else
            ret = false;

         return ret;
      }


      bool erase(T *pobj, bool flKeepObj=false)
      {
         bool  ret;
         Node<T> *pNode;


         pNode = search(pobj);

         if ( pNode && erase(pNode, flKeepObj) )
            ret = true;
         else
            ret = false;

         return ret;
      }


};  // template <class T> class ListT


#endif // _ListT_STR


