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
		cMessage *pollingModeEvent;
		cMessage *curRepMsg;
		cQueue *pQueue;
		cQueue *aQueue;
		cQueue *plQueue;
		cMessage *aLeft;
		cQueue *repPool;
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
	cancelAndDelete(pollingModeEvent);
	cancelAndDelete(curRepMsg);
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

	// replenish message pool initialize	
	repPool = new cQueue("replenishment message queue", NULL);
	for (int i = 0; i < 100; i ++) {
		repPool->insert(new cMessage("repMsg"));
	}

	lastEventStart = 0.0;	
	msgServiced = NULL;
	endServiceMsg = new cMessage("endServiceMsg");
	pauseServiceMsg = new cMessage("pauseServiceMsg");
	pollingModeEvent = new cMessage("pollingModeEvent");
	curRepMsg = NULL;
}

void RMScheduler::handleMessage(cMessage *msg)
{
	if (simTime() >= duration) {
		finish();
		cancelAndDelete(msgServiced);
		cancelAndDelete(endServiceMsg);
    } else if (msg == pauseServiceMsg) { // puase current non-background aperiodic task because of exhaustion
	    cMessage *leftMsg = msgServiced;
		simtime_t leftExecutionTime = leftMsg->getTimestamp() + lastEventStart - simTime();
	    leftMsg->setTimestamp(leftExecutionTime);
		if (leftExecutionTime == 0.0) {
			endService(leftMsg);
		} else {
			aLeft = leftMsg;
			ev << "service is paused, leftExecutionTime " << leftExecutionTime << " , aLeft " << aLeft->getName() << endl;
		}
		msgServiced = NULL;

		if (budget > 0.0) { // replenished already
			if (aLeft != NULL) {
				pollService(aLeft);
				aLeft = NULL;
			} else if (!aQueue->empty()) {
				pollService((cMessage *)aQueue->pop());
			} else {
				ev << "sporadic server is inactivated after exhausting" << endl;
				curRepMsg = NULL;
				scheduleAt(simTime(), endServiceMsg);
			}
		} else {
			curRepMsg = NULL;	
			scheduleAt(simTime(), endServiceMsg);
		}
	} else if (msg == endServiceMsg) { // task is finished, non-exhausted
		bool pollingSeq = false;
		if (msgServiced != NULL) { // non-exhausted end
			pollingSeq = (!strcmp(msgServiced->getName(), "aperiodic")) && !background;
			endService( msgServiced );
			msgServiced = NULL;
		}

		if (pollingSeq) { // polled aperiodic task is finished. non-exhausted.
			ev << "polled aperiodic task is finished, budget " << budget << endl;
			if (!aQueue->empty()) {
				pollService((cMessage *) aQueue->pop());	
			} else { // sporadic server is inactivated
				ev << "sporadic server is inactivated" << endl;
				curRepMsg = NULL;
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
	} else if (!strcmp(msg->getName(), "repMsg")) { // replenishment message
		ev << "received replenishment message. rep amount is " << msg->getTimestamp() << endl;
		budget += msg->getTimestamp().dbl();
		double diff = budget - (double)par("budget");
		if (diff >= -0.001 && diff <= 0.001) {
			ev << "budget diff " << diff << ", budget restore" << endl;	
			budget = par("budget");	
		}
		repPool->insert(msg);

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
                } else if (background) { // change to polling mode
                    cMessage *leftMsg = preempt(pollingModeEvent);
                    pollService(leftMsg);   
                } else { // sporadic server was already activated
					// do nothing	
				}
            } else { // server was idle but aperiodic task was waitng.
                pollService((cMessage *)aQueue->pop());
            }
        } else { // no wating aperiodic task
            if (msgServiced != NULL && !strcmp(msgServiced->getName(), "aperiodic") && background) 
            { // change background mode to polling mode
                cMessage *leftMsg = preempt(pollingModeEvent);
                pollService(leftMsg); 
            }
        }
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
					if (budget == 0.0) { // if budget is empty, preempt
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
			// ss
			if (!strcmp(msgServiced->getName(), "periodic")) { // periodic task on going
				if (budget > 0) {
					cMessage *leftMsg = preempt(msg); // preemptable
					pollService(msg);
					plQueue->insert(leftMsg);
				} else {
					aQueue->insert(msg);	
				}
			} else { // aperiodic task on going.
				if (background) { // change to polling mode
					cMessage *leftMsg = preempt(pollingModeEvent);
					pollService(leftMsg);
				} 
				aQueue->insert(msg);
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
	double consumingBudget;	
	if (leftServiceTime >= budget) { // budget exhausted
		scheduleAt(t + budget, pauseServiceMsg);
		consumingBudget = budget;
		budget = 0.0;
	} else { // budget enough
		scheduleAt(t + leftServiceTime, endServiceMsg);
		budget -= leftServiceTime.dbl();
		consumingBudget = leftServiceTime.dbl();
	}

	if (curRepMsg == NULL) { // sporadic server is activated now
		curRepMsg = (cMessage *)repPool->pop();
		curRepMsg->setTimestamp(consumingBudget);
		scheduleAt(t + period, curRepMsg); // setting replenishment time and amount
		ev << "sporadic server is activated. RT is " << t + period << ", consuming budget " << consumingBudget << endl;
	} else { // sporadic server was already activated
		// adding replenishment amount
		curRepMsg->setTimestamp(curRepMsg->getTimestamp() + consumingBudget);
		ev << "overloading sporadic active message. adding consuming budget " << consumingBudget <<", total consuming budget " << curRepMsg->getTimestamp() << endl;
	}
}

cMessage *RMScheduler::preempt(cMessage *msg)
{
	cMessage *canceledServiceMsg = cancelEvent(endServiceMsg);
	ev << "Canceling msg " << endServiceMsg->getName() << ", canceled service msg " << canceledServiceMsg->getName() << endl;
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
