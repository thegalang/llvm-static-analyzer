/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
*/
int main() {
	int i,j,k=0,sink=0, source=42;
	// read source from input or initilized with
	// a tainted value. e.g. source = 1234567
	i=0;
	
	if (j > 1) {
		i=10;
	}
	else {
		k = 20;
	}

	// taint path to k should be entry -> if
	if(j > 0) {
		k = source;
	} else {
		i = source;
	}
 	
 	// taint path should be entry -> if -> end
	sink = k + i;
}
