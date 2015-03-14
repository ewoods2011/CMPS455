int main(){
		Exec("../test2/blank");
		Yield();
		Exec("../test2/blank");
		Yield();
		Exec("../test2/blank");
		Yield();
		Join(Exec("../test2/blank"));	// Join ID should be 4 to 6!
	}