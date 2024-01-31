# Author: Prajas Naik and Kalash Shah

# return value of previous function is passed as a command line argument
return_value=$1

# Error Handling Logic
# For errors defined by us, we have printed appropriate errors. 
# For error codes corresponding to errno, the description is searched in the errno.h 
# and errno-base.h files and the appropriate message is extracted and printed

# function: handle_error
#       Takes an error code and prints appropriate message using the logic stated above
#   @param: Three digit error code
handle_error()
{
    error=$1
    errno_path="/"
    input_string="/"
    if [ $error -gt 200 ]; then # Handling custom errors
        if [ $error -eq 201 ]; then 
            echo "Please enter exactly five arguments. For further information, please execute the code with no arguments"
        elif [ $error -eq 202 ]; then
            echo "Write operation could not be completed, please try again later"
        elif [ $error -eq 203 ]; then 
            echo "The specifiers you entered were not recognizable, please use only '-i' and '-o'."
        else    
            echo "Unkown Error Encountered"
        fi
    else # Handling errno error codes
        if [ $error -le 34 ]; then
            errno_path="/usr/include/*/errno-base.h"
        else 
            errno_path="/usr/include/errno.h /usr/include/*/errno.h"
        fi
        input_string=$(grep -w $error $errno_path)
        first_occurrence=$(expr index "$input_string" "#") # Filtering out any extra results not used to define an error code
        input_string=${input_string:$first_occurrence}

        first_occurrence=$(expr index "$input_string" "*")
        second_occurrence=$(expr index "${input_string:$first_occurrence}" "*")  # Adjust index for the substring

        # Calculate the start and end positions of the text between '*'
        start_position=$(($first_occurrence + 1))
        end_position=$(($second_occurrence - 2))  # Adjust for the substring and 1-based index

        # Extract the text between '*'
        if [ $first_occurrence -ne 0 ] && [ $second_occurrence -ne 0 ]; then
            extracted_text="${input_string:$start_position:$end_position}"
            echo "Error: $extracted_text"
        else
            echo "Unkown Error Encountered"
        fi
    fi
}

# Handling for more than one error reported according to encoding scheme of mulitplying by 1000.
if [ $return_value -eq 0 ]; then
    echo "Success"
elif [ $return_value -gt 1000000 ]; then
    error1=$(($return_value%1000))
    handle_error $error1
    return_value=$(($return_value / 1000))
    error2=$(($return_value%1000))
    handle_error $error2
    error3=$(($return_value / 1000))
    handle_error $error3
elif [ $return_value -gt 1000 ]; then
    error1=$(($return_value%1000))
    handle_error $error1
    error2=$(($return_value / 1000))
    handle_error $error2
else
    handle_error $return_value
fi

