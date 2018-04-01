el_global::Int = 100
el_bool::Bool = false
function IsPair(val::Int)::Bool
    return val % 2 == 0
end

function WeirdStuff(val::Int)::Int
    valor::Int = val * 2;

    for i = 0:valor
        if i == 5
            println("The favorite number, ", i, ", has been reached: ", valor);
        end
        println("Is pair ", i, " the val: ", IsPair(i))
    end

    return valor * el_global
end

println("#", el_global, " is pair: ", IsPair(el_global))
println("Weird stuff from val: ", el_global, " now: ", WeirdStuff(el_global))

if el_bool == false
    println("All a lie.")
end