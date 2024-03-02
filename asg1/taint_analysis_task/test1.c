/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
*/
int main() {
   int a, c, source = 42, sink;
   
   // sink is still tainted because it was tainted in one source
   if (a > 0) {
     c += 10;
     sink = source;
  }
   else 
     c += 15;
}

