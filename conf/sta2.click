ControlSocket("TCP", 7777, VERBOSE true);

bw_shaper :: BandwidthShaper(12500000B/s) // 100Mbps
dl_shaper :: DelayShaper(0) // 0 seconds

ipv4_cl :: Classifier(
        0/45,   // IPv4
        -)      // default

ip_cl :: IPClassifier(
        src tcp port 7777,  // control socket packets
        -);                 // default

switch :: PaintSwitch();

tun :: KernelTun(192.168.2.22/24)
  //-> Print("Kernel TUN", PRINTANNO true, MAXLENGTH 250)
  //-> IPPrint("IP Print")
  -> SetTimestamp
  -> ipv4_cl;

switch[0]
  -> Queue()
  // DL Type (0x0800), SRC MAC (WiFi interface), DST MAC (Controller)
  -> EtherEncap(0x0800, 00:0E:8E:30:9E:90, 00:25:90:1D:24:D8)
  -> ToDevice(wls33);

ipv4_cl[0]
  -> ip_cl;

ipv4_cl[1]
  //-> Print("MANAGEMENT", PRINTANNO true, MAXLENGTH 250)
  -> switch;

ip_cl[0]
  //-> Print("Control Socket", PRINTANNO true, MAXLENGTH 250)
  -> switch;

ip_cl[1]
  //-> Print("DATA", PRINTANNO true, MAXLENGTH 250)
  -> Queue()
  -> dl_shaper
  -> bw_shaper
  -> Unqueue()
  -> switch;

FromDevice(wls33)
  //-> Print("From device wls33", PRINTANNO true, MAXLENGTH 250)
  -> Discard();
  //-> Queue()
  //-> DelayShaper(10)
  //-> tun;