require 'json'

#Function used to find a_starting and end keys
def check(line, needle, number)
	if line == needle
		number = 1
	end
	number = number
end

puts "Please enter document collection: "

collectionName = gets.chomp

puts "This is the entered document collection: " + collectionName

#Checks if the document collection entered exists. Otherwise, exits the program
if File.exist?(collectionName)
	puts "File exists. Performing index construction..."
	docCollection = IO.readlines(collectionName)
else
	puts "File does not exist. Now exiting the program..."
	exit
end

#The keys
key_T = ".T\n"
key_W = ".W\n"
key_B = ".B\n"
key_C = ".C\n"
key_A = ".A\n"
key_K = ".K\n"
key_N = ".N\n"
key_X = ".X\n"

#The expressions
exp_ID = /^\.I\s\d+/

#Initialize
forIndex_t = 0
forIndex_w = 0
forIndex_a = 0
index_t = 0
index_w = 0
index_a = 0
docNum = 0
t_start = 0
t_stop_w = 0
t_stop_b = 0
w_start = 0
w_stop = 0
a_start = 0
a_stop_n = 0
a_stop_k = 0

titleText = Array.new
titleIDs = Array.new
abstractText = Array.new
abstractIDs = Array.new
authorNames = Array.new
authorIDs = Array.new

docCollection.each_with_index do |line, i|

	#Keeps track of document number reached
	if (line.match(exp_ID))
		docNum += 1
	end

	
	#Finds the title text
	t_stop_w = check(line, key_W, t_stop_w)
	t_stop_b = check(line, key_B, t_stop_b)
	#If the title key is found
	if (t_start == 1)
		#If the end keys are not found
		if (t_stop_w == 0) && (t_stop_b == 0)
			if titleText[index_t] == nil
				titleText[index_t] = line
			else
				titleText[index_t] = titleText[index_t] + line
			end
		#Otherwise, end keys were found so reset values
		else
			#Takes care of empty titles
			if titleText[index_t] == nil
				titleText[index_t] = " "
			end
			
			titleIDs[index_t] = docNum
			t_start = 0
			t_stop_w = 0
			t_stop_b = 0
			index_t += 1
		end
	#Otherwise, the title key was never found
	else
		t_start = 0
		t_stop_w = 0
		t_stop_b = 0
	end
	t_start = check(line, key_T, t_start)
	
	
	#Finds the abstract text
	w_stop = check(line, key_B, w_stop)
	#If the abstract key is found
	if (w_start == 1)
		#If the end key is not found
		if (w_stop == 0)
			if abstractText[index_w] == nil
				abstractText[index_w] = line
			else
				abstractText[index_w] = abstractText[index_w] + line
			end
		#Otherwise, end key was found so reset values
		else
			abstractIDs[index_w] = docNum
			w_start = 0
			w_stop = 0
			index_w += 1
		end
	#Otherwise, the abstract key was never found
	else
		w_start = 0
		w_stop = 0
	end
	w_start = check(line, key_W, w_start)
	
	
	#Finds the author text
	a_stop_k = check(line, key_K, a_stop_k)
	a_stop_n = check(line, key_N, a_stop_n)
	#If the author key is found
	if (a_start == 1)
		#If the end keys are not found
		if (a_stop_n == 0) && (a_stop_k == 0)
			if authorNames[index_a] == nil
				authorNames[index_a] = line
			else
				authorNames[index_a] = authorNames[index_a] + line
			end
		#Otherwise, end keys were found so reset values 
		else
			authorIDs[index_a] = docNum
			a_start = 0
			a_stop_n = 0
			a_stop_k = 0
			index_a += 1
		end
	#Otherwise, the author key was never found
	else
		a_start = 0
		a_stop_n = 0
		a_stop_k = 0
	end
	a_start = check(line, key_A, a_start)
	
end

#These for loops don't affect the postings_list.txt. However, it gets rid of new lines
for i in titleText
	i = i.gsub(/[-,+'()\n]/, ' ')
	titleText[forIndex_t] = i
	forIndex_t += 1
end

for i in authorNames
	i = i.gsub(/[-,+'()\n]/, ' ')
	authorNames[forIndex_a] = i
	forIndex_a += 1
end

for i in abstractText
	i = i.gsub(/[-,+'()\n]/, ' ')
	abstractText[forIndex_w] = i
	forIndex_w += 1
end


titles = titleText.zip(titleIDs)
abstracts = abstractText.zip(abstractIDs)
authors = authorNames.zip(authorIDs)
pairs_TA = abstracts + titles
terms_TA = abstractText + titleText


term = Array.new
position = Array.new

term_position = Array.new

puts "..."

pairs_TA.each do |document,index|

	document.to_s.split(' ').each do |word|
		word = word.gsub(/[^a-zA-Z0-9\-]/,'') 
		term.push(word)
		temp = [index.to_i, document.index(word).to_i + 1]
		document.slice! word
		position.push(temp)
	end

end

puts "..."

term_position = term.zip(position)

hash = Hash.new { |hash, key| hash[key] = [] }
term_position.each { |x, y| hash[x] << y}
hash_postings = Hash[hash.sort]

File.open("postings_list.txt", 'w') do |f|

f.puts hash_postings.to_json

end

puts "Index construction complete. Now exiting."