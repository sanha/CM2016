simple FFDirectSup
{
	parameters:
		volatile double serviceTime1 @unit(s);
		volatile double duration;
	gates:
		input in[];
		output out;
}
