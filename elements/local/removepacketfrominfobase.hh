#ifndef CLICK_SIMPLEPULLELEMENT_HH
#define CLICK_SIMPLEPULLELEMENT_HH
#include <click/element.hh>
#include "empowerstainfobase.hh"

CLICK_DECLS

class RemovePacketFromInfoBase : public Element { 
	public:
		RemovePacketFromInfoBase();
		~RemovePacketFromInfoBase();
		
		const char *class_name() const	{ return "RemovePacketFromInfoBase"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PULL; }
		int configure(Vector<String>&, ErrorHandler*);
		
		Packet* pull(int);
	private:
		EmpowerStaInfoBase* infobase;
};

CLICK_ENDDECLS
#endif