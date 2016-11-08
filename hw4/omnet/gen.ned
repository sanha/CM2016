simple FFGenerator
{
	parameters:
		volatile double sendIaTime @unit(s);
		volatile double serviceTime @unit(s);
		volatile double duration;
	gates:
		output out;
}
