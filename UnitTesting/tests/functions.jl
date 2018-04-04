function func(x::Int, y::Int, z::Int)::Int
	result::Int = x + y + z

	while x < 4
	    println(x)
	    x = x + 1
		if x == 2
			println("Hola")
		end
	end
	return result
end

function addition(w::Int, x::Int, y::Int, z::Int)::Int
    return w+x+y+z;
end

println(addition(5, 5^2, 5<<2, 50/5))

println(func(1, 2, 3))

#=#
Prints
#=
60
1
Hola
2
3
6
#=#