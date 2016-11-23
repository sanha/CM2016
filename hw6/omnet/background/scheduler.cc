#include <string.h>
#include <omnetpp.h>

class RMScheduler : public cSimpleModule
{
	public:
		RMScheduler();
		virtual ~RMScheduler();
	private:
		double duration;
	protected:
		cMessage *msgServiced;
		cMessage *endServiceMsg;
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
}

int RMScheduler::periodCompFunc(cObject *a, cObject *b) 
{
	return ((cMessage *)a)->getKind() - ((cMessage *)b)->getKind();
}
void RMScheduler::initialize()
{
	duration = par("duration");	

	pQueue = new cQueue("periodic task queue", (CompareFunc)&RMScheduler::periodCompFunc);
//	pQueue = new cQueue();	
	aQueue = new cQueue("aperiodic task queue", NULL);
	plQueue = new cQueue("periodic leftover task queue", (CompareFunc)&RMScheduler::periodCompFunc);
//	plQueue = new cQueue();
	aLeft = NULL;

	lastEventStart = 0.0;	
	msgServiced = NULL;
	endServiceMsg = new cMessage("endServiceMsg");
}

void RMScheduler::handleMessage(cMessage *msg)
{
	if (simTime() >= duration) {
		finish();
		cancelAndDelete(msgServiced);
		cancelAndDelete(endServiceMsg);
    } else if (msg == endServiceMsg){
		if (msgServiced != NULL) {
			endService( msgServiced );
		}
		// task is finished
		if (!plQueue->empty()){ // periodic leftover
			startService((cMessage *) plQueue->pop());
		} else if (!pQueue->empty()) { // periodic
			startService((cMessage *) pQueue->pop());
		} else if (aLeft != NULL) { // aperiodic leftover
			startService(aLeft);
			aLeft = NULL;
		} else if (!aQueue->empty()) { // aperiodic
			startService((cMessage *) aQueue->pop());	
		} else{ // no waitng task
			msgServiced = NULL;	
		}
	} else if (msgServiced == NULL){ // server is idle
		//arrival( msg );
		startService( msg );
	} else{ // server is busy
		//arrival( msg );
		// preemption check
		// background
		if (!strcmp(msg->getName(), "periodic")) { // periodic task input
			if (!strcmp(msgServiced->getName(), "periodic")) { // periodic task on going
				if (msg->getKind() < msgServiced->getKind()) { // preempt
					cMessage *leftMsg = preempt(msg);	
					plQueue->insert(leftMsg);
				} else { // non-preemptive input
					pQueue->insert(msg);	
				}
			} else { // aperiodic task on going
				// background
				cMessage *leftMsg = preempt(msg);
				aLeft = leftMsg;
			}
		} else { // aperiodic task input
			// background
			aQueue->insert(msg);
		}
	}
}

void RMScheduler::startService(cMessage *msg)
{
	msgServiced = msg;	
	simtime_t t = simTime();
	lastEventStart = t;
	simtime_t serviceTime = msg->getTimestamp();
	ev << "Starting msg " << msg->getName() << ", simTime " << t << ", service time " << serviceTime << endl;
	scheduleAt( t + serviceTime, endServiceMsg );
}

cMessage *RMScheduler::preempt(cMessage *msg)
{
	cMessage *canceledMsg = cancelEvent(endServiceMsg);
	ev << "Canceling msg " << endServiceMsg->getName() << ", canceled msg " << canceledMsg->getName() << endl;
	cMessage *leftMsg = msgServiced;
	simtime_t timestamp = leftMsg->getTimestamp();
	leftMsg->setTimestamp(timestamp + lastEventStart - simTime());
	ev << "Preempting msg " << leftMsg->getName() << ", simTime " << simTime() << ", left service time " << timestamp + lastEventStart - simTime() << endl;
	startService(msg);

	return leftMsg;
}

void RMScheduler::endService(cMessage *msg)
{
	send( msg, "out" );
}

void RMScheduler::finish() 
{}
