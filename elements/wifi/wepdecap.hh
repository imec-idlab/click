#ifndef CLICK_WEPDECAP_HH
#define CLICK_WEPDECAP_HH
#include <click/element.hh>
#include <clicknet/ether.h>
#include <elements/wifi/rc4.hh>
CLICK_DECLS

/*
=c

WepDecap

=s Wifi

Turns 802.11 packets into ethernet packets 

=d

=e


  wifi_cl :: Classifier (0/00%0c, 
                         0/04%0c,
                         0/08%0c);

  wifi_cl [0] -> Discard; //mgt 
  wifi_cl [1] -> Discard; //ctl
  wifi_cl [2] -> wifi_decap :: WepDecap() -> ...

=a WifiEncap
 */

class WepDecap : public Element { public:
  
  WepDecap();
  ~WepDecap();

  const char *class_name() const	{ return "WepDecap"; }
  const char *processing() const	{ return AGNOSTIC; }
  
  int configure(Vector<String> &, ErrorHandler *);
  bool can_live_reconfigure() const	{ return true; }

  Packet *simple_action(Packet *);


  void add_handlers();


  bool _debug;
  bool _strict;


  u_int32_t iv;
  struct rc4_state _rc4;
  u_int8_t key[16];
  int keylen;
 private:

};

CLICK_ENDDECLS
#endif
