#include <string.h>
#include <omnetpp.h>

class FFPartitionSup : public cSimpleModule
{
	private:
		double duration;
	public:
		FFPartitionSup();
		virtual ~FFPartitionSup();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module(FFPartitionSup);

FFPartitionSup::FFPartitionSup()
{}

FFPartitionSup::~FFPartitionSup()
{}

void FFPartitionSup::initialize()
{
	duration = par("duration");
}

void FFPartitionSup::handleMessage(cMessage *msg)
{
	if (simTime() >= duration) {
		finish();
	} else {
		delete msg;
		cMessage *m = new cMessage("job from partition sup");
		
		int idx = intuniform(0, gateSize("out") - 1);

		switch (idx) {
			case 0:
				m->setTimestamp(par("serviceTime2"));
				break;
			case 1:
				m->setTimestamp(par("serviceTime3"));
				break;
			case 2:
				m->setTimestamp(par("serviceTime4"));
				break;
			case 3:
				break;
			default:
				ev << "partition input index err!" << endl;
				break;		
		}

		send(m, "out", idx);
	}
}

void FFPartitionSup::finish() 
{}
