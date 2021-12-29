#ifndef __DEPENDANCIES__
#define __DEPENDANCIES__

#define STRICT

#include "HWNDList.h"

//********************************************************************************************************
// How to use :
// You are meant to inherit any class that needs dependant windows to be sent messages upon changes from thi
// In order to send custom messages with your own message types, overload ChangeMade with something like :
//
// void	MYCLASS::ChangeMade(void)
// {
//		Dependants.SendMessage(WM_MYMESSAGE,MYPARAM1,MYPARAM2);
// }
//********************************************************************************************************
// Notes on use :
// The memory usage per empty (probably the most common situation) HWND list is just 2 longs : 8 bytes. 

class HWNDDependancies
	{	public:
			HWNDList	Dependants;

			// Add this window into the list of dependants
			void	Add_Dependant_HWND(HWND hWnd);

			// Delete this window from the list of dependants
			void	Delete_Dependant_HWND(HWND);

			// Delete all dependants
			// Should be used with caution, unless you know that no other windows need it
			void	Delete_AllDependants(void);

			// Useful to internally send different message types
			void	SendMessage(int message, LPARAM lParam=0,WPARAM wParam=0) 
							{ Dependants.SendMessage(message,lParam,wParam); }

			// Send the default message, if you don't want the default, overload this.
			virtual void	ChangeMade(void);

			// Temporarily Switch Messages off for this class, useful if many changes are getting made at once
			void	MessagesOff(void)	{	Dependants.MessagesOff();	}

			// Switch Messages back on
			void	MessagesOn(void)	{	Dependants.MessagesOn();	}

			// Merge Another List In
			void	Merge(HWNDDependancies *With);

			~HWNDDependancies(void)	{ Delete_AllDependants(); }
	};

#endif