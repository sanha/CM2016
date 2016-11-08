#include <string.h>
#include <omnetpp.h>

class FFSink : public cSimpleModule
{
	private:
		double duration;
	public:
		FFSink();
		virtual ~FFSink();
	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
};

Define_Module( FFSink );

FFSink::FFSink()
{};

FFSink::~FFSink()
{}

void FFSink::initialize()
{ duration = par("duration");}

void FFSink::handleMessage(cMessage *msg)
{
	if (simTime() >= duration) {
		finish();
	}
//	ev << simTime().dbl() << ", " << msg->getCreationTime().dbl() << endl;
	delete msg;
}

void FFSink::finish()
{}
