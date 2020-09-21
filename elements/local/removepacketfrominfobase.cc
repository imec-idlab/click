#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>

#include "removepacketfrominfobase.hh"

CLICK_DECLS
RemovePacketFromInfoBase::RemovePacketFromInfoBase()
{}

RemovePacketFromInfoBase::~ RemovePacketFromInfoBase()
{}

int RemovePacketFromInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
    EmpowerStaInfoBase *tempInfobase;

    int result = Args(conf, this, errh).read_m("INFOBASE", ElementCastArg("EmpowerStaInfoBase"), tempInfobase).complete();

    if (result < 0) return result;

    infobase = tempInfobase;
	return 0;
}

Packet* RemovePacketFromInfoBase::pull(int){
    // click_chatter("Removing Packet from InfoBase");

	Packet* p = input(0).pull();
	if(p == 0){
		return 0;
	}

    int dscp = 0;
    click_ether *eh = (click_ether *) p->data();
    if (ntohs(eh->ether_type) == 0x0800) {
        const click_ip *ip = p->ip_header();
        dscp = ip->ip_tos >> 2;
        
        infobase->process_packet_dequeue(dscp, Timestamp::now());
        return p;
    } else {
        p->kill();
        return 0;
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RemovePacketFromInfoBase)