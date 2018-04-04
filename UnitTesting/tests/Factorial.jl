function factorial(z::Int)::Int
	if z == 1 || z == 0
		return 1
	end
	return factorial(z-1) * z
end

println(factorial(5))


# prints 120
