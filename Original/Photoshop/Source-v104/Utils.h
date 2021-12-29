#ifndef _UTCLASS_
#define _UTCLASS_

#define Alloc2D(Variable,Type,n1,n2)									\
		{ Variable=(Type **)malloc(sizeof(Type *)*n1);					\
          Variable[0]=(Type *)malloc(sizeof(Type)*n1*n2);				\
		  for(int A2D=1;A2D<n1;A2D++)									\
				Variable[A2D]=Variable[A2D-1]+n2;						\
		}

#define Free2D(Variable,n1)						\
		{ free(Variable[0]);					\
          free(Variable);						\
		}

#endif