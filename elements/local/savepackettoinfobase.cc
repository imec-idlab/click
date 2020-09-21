#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>


#include "savepackettoinfobase.hh"

CLICK_DECLS
SavePacketToInfoBase::SavePacketToInfoBase()
{}

SavePacketToInfoBase::~ SavePacketToInfoBase()
{}

int SavePacketToInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
	EmpowerStaInfoBase* tempInfobase;

	int result = Args(conf, this, errh).read_m("INFOBASE", ElementCastArg("EmpowerStaInfoBase"), tempInfobase).complete();

	if (result < 0) return result;

	infobase = tempInfobase;

	return 0;
}

void SavePacketToInfoBase::push(int, Packet *p){
	click_chatter("Saving a packet to Infobase");

    int dscp = 0;
    click_ether *eh = (click_ether *) p->data();
    if (ntohs(eh->ether_type) == 0x0800) {
        const click_ip *ip = p->ip_header();
        dscp = ip->ip_tos >> 2;	
        
        infobase->process_packet_enqueue(dscp, Timestamp::now());
        output(0).push(p);

    } else {
        p->kill();
    }

}

CLICK_ENDDECLS
EXPORT_ELEMENT(SavePacketToInfoBase)