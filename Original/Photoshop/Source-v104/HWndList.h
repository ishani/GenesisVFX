#ifndef __VFX__HWNDLIST__
#define __VFX__HWNDLIST__

#include <windows.h>
#include "Messages.h"

#define STRICT

class HWNDList
	{	public:
			unsigned char	MessagesEnabled;	// Turn this off to Temporarily stop Message Passing
			int		NoItems;
			HWND	*List;

			// Add an Item To List ... Duplicates do not get added (!)
			void Add(HWND item)
				{	if (item==NULL) return;

					for(int i=0;i!=NoItems;i++)
					if (List[i]==item) return;

					NoItems++;
					List=(HWND *)realloc(List,sizeof(HWND)*NoItems);

					List[NoItems-1]=item;
				}

			// Delete An item From The List ... If not there it still works
			void Delete(HWND item)
				{	if (item==NULL) return;
			
					for(int i=0;i!=NoItems;i++)
					if (List[i]==item) 
						{	for(int j=i;j<NoItems-1;j++) List[j]=List[j+1];

							NoItems--;
							List=(HWND *)realloc(List,sizeof(HWND)*NoItems);
							return;
						}
				}

			// Disable The Windows
			void Disable(void);

			// Enable The Windows
			void Enable(int T=0);

			// Send a message to all items in the list
			BOOL SendMessage(int,LPARAM,WPARAM);

			// Switch Message Handling On	: NOTE, This Counts Ons/Offs
			void MessagesOn(void)	{	MessagesEnabled--; 
										if (MessagesEnabled==255) 
											{	throw("MessagesOn Error : HWnd Dependancies wrap around. Messages Reset.");
												MessagesEnabled=0;
											}
									}

			// Switch message Handling Off
			void MessagesOff(void)	{	MessagesEnabled++; 
										if (MessagesEnabled==255) 
											{	throw("MessagesOff Error : HWnd Dependancies wrap around. Messages Reset.");
												MessagesEnabled=0;
											}
									}

			// Delete All hwnds
			void DeleteAll(void);

			HWNDList();
			~HWNDList();
	};

#endif