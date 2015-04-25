int main(){
		Write("A MINOR", 11, 1);
		Exec("../test2/blank");
		Write("TESTING", 11, 1);
		Yield();
		Write("TESTING2", 11, 1);
		Exec("../test2/blank");
		Yield();
		Exec("../test2/blank");
		Yield();
		Join(Exec("../test2/blank"));	// Join ID should be 4 to 6!
	}
