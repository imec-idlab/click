#ifndef CLICK_RECORDDELAY_HH
#define CLICK_RECORDDELAY_HH
#include <click/element.hh>
#include "empowerstainfobase.hh"
CLICK_DECLS

class SavePacketToInfoBase : public Element { 
	public:
		SavePacketToInfoBase();
		~SavePacketToInfoBase();
		
		const char *class_name() const	{ return "SavePacketToInfoBase"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		EmpowerStaInfoBase* infobase;
};

CLICK_ENDDECLS
#endif