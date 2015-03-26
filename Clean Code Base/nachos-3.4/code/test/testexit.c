int main(){
			int i = 0;
			for ( ; i < 10 ; i++ ){
				Exec("../test/test7");
				if (i % 2 == 0)
					Yield();
			}
Exit(0);
		}