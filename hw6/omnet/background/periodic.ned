simple RMPeriodic
{
	parameters:
		volatile double period;
		volatile double executionTime @unit(s);
		volatile double duration;
	gates:
		output out;
}
