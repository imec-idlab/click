#ifndef CLICK_EMPOWERSTAINFOBASE_HH
#define CLICK_EMPOWERSTAINFOBASE_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashmap.hh>
CLICK_DECLS


// Key: DSCP value (int)
typedef HashMap<int, Vector<Timestamp> > DSCPTimestampEnqueuedPacketsMap;

// Key: DSCP value (int) 
typedef HashMap<int, Vector<Timestamp> > DSCPDelayPacketsMap;

// Key: DSCP value (int)
typedef HashMap<int, Timestamp> DSCPQueueDelayMap;

class EmpowerStaInfoBase : public Element { 
    public:
        EmpowerStaInfoBase();
        ~EmpowerStaInfoBase();

        const char *class_name() const	{ return "EmpowerStaInfoBase"; }
        const char *port_count() const	{ return "0"; }
        const char *processing() const	{ return AGNOSTIC; }
        int configure(Vector<String>&, ErrorHandler*);

        // DSCP (int) and timestamp (int)
        void process_packet_enqueue(int, Timestamp);

        // DSCP (int) and timestamp (int)
        void process_packet_dequeue(int, Timestamp);

        // DSCP (int) returns the queue delay in Timestamp format
        Timestamp get_queue_delay();

        void run_timer(Timer *);

        void add_handlers();


    private:
        bool _debug;

        uint32_t digit;

        unsigned int _period;
        
        Timer _timer;

        /*
         * Structure for holding the last TX packets timestamp according to its DSCP values
         */
        DSCPTimestampEnqueuedPacketsMap dscp_timestamp_enqueued_pkts_map;

        /* 
         * Structure for holding delay info per DSCP (slice)
         */
        DSCPQueueDelayMap dscp_queue_delay_map;

        /*
         * Structure for holding the last transmitted packet delays per DSCP (Slice)
         */
        DSCPDelayPacketsMap dscp_delay_packets_map;

        static String read_handler(Element *, void *);

};

CLICK_ENDDECLS
#endif