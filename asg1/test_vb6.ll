; ModuleID = 'test_vb6.c'
source_filename = "test_vb6.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %w = alloca i32, align 4
  %z = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %0 = load i32, i32* %a, align 4
  %1 = load i32, i32* %b, align 4
  %cmp = icmp ne i32 %0, %1
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %2 = load i32, i32* %a, align 4
  %3 = load i32, i32* %b, align 4
  %cmp1 = icmp sgt i32 %2, %3
  br i1 %cmp1, label %if.then, label %if.else

if.then:                                          ; preds = %while.body
  %4 = load i32, i32* %b, align 4
  %5 = load i32, i32* %a, align 4
  %sub = sub nsw i32 %4, %5
  store i32 %sub, i32* %x, align 4
  %6 = load i32, i32* %a, align 4
  %7 = load i32, i32* %b, align 4
  %add = add nsw i32 %6, %7
  store i32 %add, i32* %y, align 4
  %8 = load i32, i32* %y, align 4
  %9 = load i32, i32* %w, align 4
  %add2 = add nsw i32 %8, %9
  store i32 %add2, i32* %z, align 4
  br label %if.end

if.else:                                          ; preds = %while.body
  %10 = load i32, i32* %a, align 4
  %11 = load i32, i32* %b, align 4
  %add3 = add nsw i32 %10, %11
  store i32 %add3, i32* %x, align 4
  %12 = load i32, i32* %b, align 4
  %13 = load i32, i32* %a, align 4
  %sub4 = sub nsw i32 %12, %13
  store i32 %sub4, i32* %y, align 4
  %14 = load i32, i32* %y, align 4
  %15 = load i32, i32* %a, align 4
  %add5 = add nsw i32 %14, %15
  store i32 %add5, i32* %z, align 4
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %16 = load i32, i32* %retval, align 4
  ret i32 %16
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
