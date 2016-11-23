simple RMAperiodic
{
	parameters:
		volatile double period;
		volatile double interval @unit(s);
		volatile double executionTime @unit(s);
		volatile double duration;
	gates:
		output out;
}
