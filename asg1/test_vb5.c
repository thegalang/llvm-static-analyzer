int main() {
   int a, b, x, y, w, z;
   
   // b - a is not very busy because a was reinitialized in the third block
   if (a > b) {
     x = b - a;
     y = a + b;
   }
   else if(b < a){
    x = a + b;
    y = b - a;
   } 
   else {
    w = a + b;
    a = w + b;
    z = b - a;
   }
}

