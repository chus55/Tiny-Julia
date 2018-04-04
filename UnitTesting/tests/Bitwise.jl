#=
*******************************************
 * File: Bitwise
 * A test program for relational operators
 *******************************************
=#
x::Int = 10
y::Int = 20
boolValue::Bool = true;

 println("x = ",x )
 println("y = ",y )
 println("boolValue = ",boolValue )

 println("x < y && x > y = ", x < y && x > y  )

 println("x < y || x > y = ", x < y || x > y  )

 println("~x = ",~x ); println("~y = ", ~y)

 println("!boolValue = ",!boolValue )

 println("x & y = ",x & y )

 println("x | y = ",x | y )

 println("x $ y = ",x $ y )

 println("-8 >> 3 = ",-8 >> 3 )

 println("2 << 4 = ", 2 << 4)

 z::Bool = !true
 println(~z)

#=# Should Print

x = 10
y = 20
boolValue = true
x < y && x > y = false
x < y || x > y = true
~x = -11
~y = -21
!boolValue = false
x & y = 0
x | y = 30
x $ y = 30
-8 >> 3 = -1
2 << 4 = 32
true
#=#
