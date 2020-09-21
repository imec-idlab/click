#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "empowerstainfobase.hh"

CLICK_DECLS
EmpowerStaInfoBase::EmpowerStaInfoBase() : _timer(this) {}

EmpowerStaInfoBase::~ EmpowerStaInfoBase() {}

int EmpowerStaInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
    _timer.initialize(this);
    _timer.schedule_now();
	return Args(conf, this, errh).read_m("PERIOD", _period).complete();
}


void EmpowerStaInfoBase::run_timer(Timer *) {
    click_chatter("Calculating stats....");


    // Compute the average queue delay
    for (DSCPDelayPacketsMap::iterator i = dscp_delay_packets_map.begin(); i != dscp_delay_packets_map.end(); i++) {

        Timestamp sum = Timestamp();
        Timestamp avg = Timestamp();

        // Iterate over the vector of computed delays
        for (Vector<Timestamp>::const_iterator j = i.value().begin(); j != i.value().end(); j++) {
            sum += *j;
        }

        // If there was a sum
        if (sum) {
            avg = sum / i.value().size();
        }

        // Update the avg delay
        dscp_queue_delay_map.insert(i.key(), avg);
        click_chatter("Delay for %d: %d", i.key(), avg);
    }

    // Reset delay packets map
    for (DSCPDelayPacketsMap::iterator i = dscp_delay_packets_map.begin(); i != dscp_delay_packets_map.end(); i++) {
        i.value().clear();
    }


    _timer.schedule_after_msec(_period);
}

// DSCP (int) and timestamp (int)
void EmpowerStaInfoBase::process_packet_enqueue(int dscp, Timestamp timestamp) {

    // save all packets with the same "dscp" value, so all packets on the STA 
    // are treated as a single "slice" or station
    dscp = 0;

    if (_debug) {
        click_chatter("%{element} :: %s :: process packet ENqueue for DSCP: %d and Timestamp: %s!",
                      this,
                      __func__,
                      dscp,
                      timestamp.unparse().c_str());
    }

    DSCPTimestampEnqueuedPacketsMap :: Pair *crr_dscp_timestamp_enqueued_pkts_pair = dscp_timestamp_enqueued_pkts_map.find_pair(dscp);

    // If DSCP exists in timestamp enqueued packets map
    if (crr_dscp_timestamp_enqueued_pkts_pair) {
        if (_debug) {
            click_chatter("%{element} :: %s :: Enqueued packets vector size is: %d",
                          this,
                          __func__,
                          crr_dscp_timestamp_enqueued_pkts_pair->value.size());
        }

        // Include packet timestamp at the end of the vector
        crr_dscp_timestamp_enqueued_pkts_pair->value.push_back(timestamp);
    } else {
        if (_debug){
            click_chatter("%{element} :: %s :: Initializing Timestamp vector for DSCP: %d",
                          this,
                          __func__,
                          dscp);
        }

        // Create vector for packet timestamps
        Vector<Timestamp> timestamp_enqueued_pkts_vector;

        // Inserting current time into it
        timestamp_enqueued_pkts_vector.push_back(timestamp);

        // Insert the vector of timestamps into DSCP hashmap
        dscp_timestamp_enqueued_pkts_map.insert(dscp, timestamp_enqueued_pkts_vector);
    }
    

}

// DSCP (int) and timestamp (int)
void EmpowerStaInfoBase::process_packet_dequeue(int dscp, Timestamp timestamp) {
    if (_debug){
        click_chatter("%{element} :: %s :: process packet DEqueue for DSCP: %d and Timestamp: %s!",
                      this,
                      __func__,
                      dscp,
                      timestamp.unparse().c_str());
    }

    DSCPTimestampEnqueuedPacketsMap :: Pair *crr_dscp_timestamp_enqueued_pkts_pair = dscp_timestamp_enqueued_pkts_map.find_pair(dscp);
    assert(crr_dscp_timestamp_enqueued_pkts_pair);

    // Check if the DSCP exists in the hashmap
    if (crr_dscp_timestamp_enqueued_pkts_pair) {

        // Check if there is at least one packet in the vector
        if (crr_dscp_timestamp_enqueued_pkts_pair->value.size() > 0) {

            Vector<Timestamp>::const_iterator first_timestamp = crr_dscp_timestamp_enqueued_pkts_pair->value.begin();

            // Calculate queue delay for the DSCP
            Timestamp crr_queue_delay = timestamp - *first_timestamp;

            if (_debug){
                click_chatter("%{element} :: %s :: First timestamp: %s vs current: %s, calculated: %s, unparse_interval: %s!",
                              this,
                              __func__,
                              first_timestamp->unparse().c_str(),
                              timestamp.unparse().c_str(),
                              crr_queue_delay.unparse().c_str(),
                              crr_queue_delay.unparse_interval().c_str());
            }

            // Add delay into the DSCP delay packets map
            DSCPDelayPacketsMap :: Pair *crr_dscp_delay_pkts_pair = dscp_delay_packets_map.find_pair(dscp);

            if (crr_dscp_delay_pkts_pair) {

                // Adding the crr delay into the map
                crr_dscp_delay_pkts_pair->value.push_back(crr_queue_delay);

                if (_debug){
                    click_chatter("%{element} :: %s :: DSCP: %d and size(): %d!",
                                  this,
                                  __func__,
                                  dscp,
                                  crr_dscp_delay_pkts_pair->value.size());
                }

            } else {
                // Create vector and add the delay into it
                Vector<Timestamp> crr_delays_vector;
                crr_delays_vector.push_back(crr_queue_delay);

                // Insert into the map
                dscp_delay_packets_map.insert(dscp, crr_delays_vector);
            }

            // Remove first timestamp from the vector
            crr_dscp_delay_pkts_pair->value.pop_front();

        } else {
            if (_debug) {
                click_chatter(
                        "%{element} :: %s :: error: skipping packet DEqueue because timestamp vector is empty or not valid!",
                        this, __func__);
            }

        }
    } else {
        if (_debug){
            click_chatter("%{element} :: %s :: error: packet DEqueue hashmap pair not valid!", this, __func__);
        }

    }

}

// DSCP (int) returns the queue delay in Timestamp format
Timestamp EmpowerStaInfoBase::get_queue_delay() {

    int dscp = 0;
    // Getting the average latency from the map
    DSCPQueueDelayMap :: Pair *crr_dscp_queue_delay_pair = dscp_queue_delay_map.find_pair(dscp);

    if (crr_dscp_queue_delay_pair) {
        return crr_dscp_queue_delay_pair->value;
    } else {
        return Timestamp(); // returns empty timestamp
    }
}   

enum {
    H_DEBUG,
    H_PERIOD
};

String EmpowerStaInfoBase::read_handler(Element *e, void *thunk) {
    EmpowerStaInfoBase *td = (EmpowerStaInfoBase *) e;
    return String(td->get_queue_delay()) + "\n";
}


void EmpowerStaInfoBase::add_handlers() {
    add_read_handler("delay", read_handler, (void *) H_PERIOD);
}



CLICK_ENDDECLS
EXPORT_ELEMENT(EmpowerStaInfoBase)