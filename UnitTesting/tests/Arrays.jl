y::Array{Int} = [3, 2, 1]
x::Array{Int} = [15, 34, 25, y[2]^3]

for a = 1:4
    if x[a] > 25
        println("Mayor a 25 ", x[a])
    elseif x[a] < 25
        println("Menor a 25 ", x[a])
    else
        println("Igual a 25 ", x[a])
    end
end

#=Prints
Menor a 25 15
Mayor a 25 34
Igual a 25 25
Menor a 25 8
=#