#!/bin/bash
#This regulates the input in script
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <department_files>... employee <employee_ids>..."
    exit 1
fi

status=("Present" "Absent" "Leave")

#Declare an array to calculate the number of occurrences of a status.
declare -A counts
#Declare another array to store the name-ID
declare -A names

#Aimed to store each id and name, forming relation by associate array.

while read -r id name; do 
    if [[ -z "$id" ]]; then
        continue
    fi
    names["$id"]="$name"
done < employees.dat

#Initialize total counts
declare -A total_counts
declare -A department_status

#Find the index of "employee"
employee_index=-1
for i in $(seq 1 $#); do
    if [[ "${!i}" == "employee" ]]; then
        employee_index=$i
        break
    fi
done

#If "employee" was not found, exit
if [ $employee_index -eq -1 ]; then
    echo "Error: 'employee' keyword not found."
    exit 1
fi

# Process department files
department_files=("${@:1:$employee_index-1}")

# Process employee IDs
employee_ids=("${@:employee_index+1}")

#For each employee ID
for employee_id in "${employee_ids[@]}"; do
 
    total_counts=(["Present"]=0 ["Absent"]=0 ["Leave"]=0)
    #For each file
    for file in "${department_files[@]}"; do
    department_status["$file"]="N/A"

    if [[ -f "$file" ]]; then
        first_line=true  
        while read -r line || [[ -n "$line" ]]; do
            #forget about the first line
            if $first_line; then
                first_line=false
                continue
            fi
           
            if [[ "$line" == "$employee_id "* ]]; then
                status=$(echo "$line" | awk '{print $2}') # because the status is in the second column
                #Switch the status 
                case "$status" in
                    Present)
                        ((total_counts["Present"]++))
                        department_status["$file"]="Present"
                        ;;
                    Absent)
                        ((total_counts["Absent"]++))
                        department_status["$file"]="Absent"
                        ;;
                    Leave)
                        ((total_counts["Leave"]++))
                        department_status["$file"]="Leave"
                        ;;
                esac
            fi
        done < "$file"
    fi
    done

    # output attendance report
    echo "Attendance Report for $employee_id ${names["$employee_id"]}"
    for filename in "${!department_status[@]}"; do
        echo "$filename: ${department_status[$filename]}"
    done

    #output totals
    echo "Total Present Days: ${total_counts["Present"]}"
    echo "Total Absent Days: ${total_counts["Absent"]}"
    echo "Total Leave Days: ${total_counts["Leave"]}"
    echo ""

    
done