ControlSocket("TCP", 7777);

bw_shaper :: BandwidthShaper(187500B/s) // 1.5Mbps

tun :: KernelTun(192.168.2.23/24)
  -> Print("Kernel TUN", PRINTANNO true, MAXLENGTH 250)
  -> IPPrint("IP Print")
  -> SetTimestamp
  -> Queue()
  //-> DelayShaper(5)
  -> bw_shaper
  // DL Type (0x0800), SRC MAC (WiFi interface), DST MAC (Controller)
  -> EtherEncap(0x0800, 00:0E:8E:30:9C:DB, 00:25:90:1D:24:D8)
  -> ToDevice(wls33);

FromDevice(wls33) 
  -> Print("From device wls33", PRINTANNO true, MAXLENGTH 250)
  -> Discard();
  //-> Queue()
  //-> DelayShaper(10)
  //-> tun;
