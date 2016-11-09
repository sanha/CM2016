simple FFPartitionSup
{
	parameters:
		volatile double serviceTime2 @unit(s);
		volatile double serviceTime3 @unit(s);
		volatile double serviceTime4 @unit(s);
		volatile double duration;
	gates:
		input in;
		output out[];
}
