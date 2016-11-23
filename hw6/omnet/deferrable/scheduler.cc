#include <string.h>
#include <omnetpp.h>

class RMScheduler : public cSimpleModule
{
	public:
		RMScheduler();
		virtual ~RMScheduler();
	private:
		double duration;
		double budget;
		double period;
		bool background;
	protected:
		cMessage *msgServiced;
		cMessage *endServiceMsg;
		cMessage *pauseServiceMsg;
		cMessage *apCheckEvent;
		cMessage *pollingModeEvent;
		cQueue *pQueue;
		cQueue *aQueue;
		cQueue *plQueue;
		cMessage *aLeft;
		simtime_t lastEventStart;
		CompareFunc comp;
		virtual int periodCompFunc(cObject *a, cObject *b);
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void startService(cMessage *msg);
		virtual void pollService(cMessage *msg);
		virtual cMessage *preempt(cMessage *msg);
		virtual void endService(cMessage *msg);
		virtual void finish();
};

Define_Module(RMScheduler);

RMScheduler::RMScheduler()
{}

RMScheduler::~RMScheduler()
{
	cancelAndDelete(msgServiced);
	cancelAndDelete(endServiceMsg);
	cancelAndDelete(pauseServiceMsg);
	cancelAndDelete(apCheckEvent);
	cancelAndDelete(pollingModeEvent);
}

int RMScheduler::periodCompFunc(cObject *a, cObject *b) 
{
	return ((cMessage *)a)->getKind() - ((cMessage *)b)->getKind();
}
void RMScheduler::initialize()
{
	duration = par("duration");	
	budget = par("budget");	
	period = par("period");	
	background = false;	

	pQueue = new cQueue("periodic task queue", (CompareFunc)&RMScheduler::periodCompFunc);
	aQueue = new cQueue("aperiodic task queue", NULL);
	plQueue = new cQueue("periodic leftover task queue", (CompareFunc)&RMScheduler::periodCompFunc);
	aLeft = NULL;

	lastEventStart = 0.0;	
	msgServiced = NULL;
	endServiceMsg = new cMessage("endServiceMsg");
	pauseServiceMsg = new cMessage("pauseServiceMsg");
	apCheckEvent = new cMessage("apCheckEvent");
	pollingModeEvent = new cMessage("pollingModeEvent");
	scheduleAt(0.0, apCheckEvent);
}

void RMScheduler::handleMessage(cMessage *msg)
{
	if (simTime() >= duration) {
		finish();
		cancelAndDelete(msgServiced);
		cancelAndDelete(endServiceMsg);
    } else if (msg == pauseServiceMsg) { // puase current (aperiodic) task because of exhaustion
	    cMessage *leftMsg = msgServiced;
		simtime_t leftExecutionTime = leftMsg->getTimestamp() + lastEventStart - simTime();
	    leftMsg->setTimestamp(leftExecutionTime);
		if (leftExecutionTime == 0.0) {
			endService(leftMsg);
		} else {
			aLeft = leftMsg;
		}
		msgServiced = NULL;
		scheduleAt(simTime(), endServiceMsg);
	} else if (msg == endServiceMsg) { // task is finished
		bool pollingSeq = false;
		if (msgServiced != NULL) {
			pollingSeq = (!strcmp(msgServiced->getName(), "aperiodic")) && !background;
			endService( msgServiced );
			msgServiced = NULL;
		}

		if (pollingSeq) { // polled aperiodic task is finished. not exhausted.
			ev << "polled aperiodic task is finished, budget " << budget << endl;
			if (!aQueue->empty()) {
				pollService((cMessage *) aQueue->pop());	
			} else {
				scheduleAt(simTime(), endServiceMsg);
			}
		} else { // general task is finished
			if (!plQueue->empty()){ // periodic leftover
				startService((cMessage *) plQueue->pop());
			} else if (!pQueue->empty()) { // periodic
				startService((cMessage *) pQueue->pop());
			} else if (aLeft != NULL) { // aperiodic leftover
				background = true; // background mode. not using budget.	
				startService(aLeft);
				aLeft = NULL;
			} else if (!aQueue->empty()) { // aperiodic
				background = true; // background mode. not using budget.	
				startService((cMessage *) aQueue->pop());	
			} else { // no waitng task
				msgServiced = NULL;	
			}
		}
	} else if (msg == apCheckEvent) { // aperiodic task check
		budget = par("budget");	
		if (aLeft != NULL) {
			if (msgServiced != NULL) {
				cMessage *leftMsg = preempt(aLeft);
				plQueue->insert(leftMsg);
			}
			pollService(aLeft);
			aLeft = NULL;
		} else if (!aQueue->empty()) {
            if (msgServiced != NULL) {
                if(!strcmp(msgServiced->getName(), "periodic")) { // periodic task is interrupted
                    cMessage *aMsg = (cMessage *)aQueue->pop();
                    cMessage *leftMsg = preempt(aMsg);
                    plQueue->insert(leftMsg);
                    pollService(aMsg);
                } else { // change to polling mode
                    cMessage *leftMsg = preempt(pollingModeEvent);
                    pollService(leftMsg);   
                }
            } else { // server was idle but aperiodic task was waitng.
                pollService((cMessage *)aQueue->pop());
            }
        } else { // no wating aperiodic task
            if (msgServiced != NULL && !strcmp(msgServiced->getName(), "aperiodic")) 
            { // change background mode to polling mode
                cMessage *leftMsg = preempt(pollingModeEvent);
                pollService(leftMsg); 
            }
        }
        scheduleAt(simTime() + period, apCheckEvent);
	} else if (msgServiced == NULL){ // server is idle, external input
		//arrival( msg );
		if (!strcmp(msg->getName(), "aperiodic")) {
			background = true;	
		}
		startService( msg );
	} else { // server is busy
		//arrival( msg );
		// preemption check
		if (!strcmp(msg->getName(), "periodic")) { // periodic task input
			if (!strcmp(msgServiced->getName(), "periodic")) { // periodic task on going
				if (msg->getKind() < msgServiced->getKind()) { // preempt
					cMessage *leftMsg = preempt(msg);	
					startService(msg);
					plQueue->insert(leftMsg);
				} else { // non-preemptive input
					pQueue->insert(msg);	
				}
			} else { // aperiodic task on going
				if (background) { // in background mode
					if (budget == 0.0) { // if budget is empty,  preemption
						cMessage *leftMsg = preempt(msg);
						startService(msg);
						aLeft = leftMsg;
					} else { // if budget is left, change to polling mode
						cMessage *leftMsg = preempt(pollingModeEvent);
						pollService(leftMsg);
						pQueue->insert(msg);
					}
				} else { // in polling mode, non-preemtive
					pQueue->insert(msg);	
				}
			}
		} else { // aperiodic task input
			// ds
			if (!strcmp(msgServiced->getName(), "periodic")) { // periodic task on going
				cMessage *leftMsg = preempt(msg); // preemptable
				pollService(msg);
				plQueue->insert(leftMsg);
			} else { // aperiodic task on going.
				if (background) { // change to polling mode
					cMessage *leftMsg = preempt(pollingModeEvent);
					pollService(leftMsg);
				} else {
					aQueue->insert(msg);
				}
			}
		}
	}
}

void RMScheduler::startService(cMessage *msg)
{
	msgServiced = msg;	
	simtime_t t = simTime();
	lastEventStart = t;
	simtime_t serviceTime = msg->getTimestamp();
	ev << "Starting msg " << msg->getName() << ", simTime " << t << ", service time " << serviceTime << ", background " << background <<", budget " << budget <<  endl;
	scheduleAt( t + serviceTime, endServiceMsg );
}

void RMScheduler::pollService(cMessage *msg)
{
	background = false;
	msgServiced = msg;	
	simtime_t t = simTime();
	lastEventStart = t;
	simtime_t leftServiceTime = msg->getTimestamp();
	ev << "Polling msg " << msg->getName() << ", simTime " << t << ", service time " << msg->getTimestamp() << ", budget " << budget << ", background " << background << endl;
	if (leftServiceTime >= budget) { // budget exhausted
		scheduleAt(t + budget, pauseServiceMsg);
		budget = 0.0;
	} else { // budget enough
		scheduleAt(t + leftServiceTime, endServiceMsg);
		budget -= leftServiceTime.dbl();
	}
}

cMessage *RMScheduler::preempt(cMessage *msg)
{
	cMessage *canceledServiceMsg = cancelEvent(endServiceMsg);
	cMessage *canceledPauseMsg = cancelEvent(pauseServiceMsg);
	ev << "Canceling msg " << endServiceMsg->getName() << ", canceled service msg " << canceledServiceMsg->getName() << ", canceled paush msg " << canceledPauseMsg->getName() << endl;
	cMessage *leftMsg = msgServiced;
	simtime_t timestamp = leftMsg->getTimestamp();
	leftMsg->setTimestamp(timestamp + lastEventStart - simTime());
	ev << "Preempting msg " << leftMsg->getName() << ", simTime " << simTime() << ", left service time " << timestamp + lastEventStart - simTime() << endl;

	return leftMsg;
}

void RMScheduler::endService(cMessage *msg)
{
	send( msg, "out" );
}

void RMScheduler::finish() 
{}
