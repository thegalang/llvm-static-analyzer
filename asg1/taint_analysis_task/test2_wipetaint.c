/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
*/
int main() {
   int a, c, d, source = 42, sink;
   
   // source is not tainted because it was cleared with a + c. d is tainted
   if (a > 0) {
     c += 10;
     source = c;
  }
   else {
     d = source;
     c += 15; 
     source = a + c;
   }
}

