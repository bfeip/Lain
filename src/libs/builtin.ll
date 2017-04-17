; ModuleID = 'builtin.c'
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind
define void @print(i8* %str) #0 {
  %1 = alloca i8*, align 8
  store i8* %str, i8** %1, align 8
  %2 = load i8*, i8** %1, align 8
  %3 = call i32 @puts(i8* %2)
  ret void
}

declare i32 @puts(i8*) #1