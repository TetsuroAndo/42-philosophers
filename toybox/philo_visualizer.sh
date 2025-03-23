#!/bin/bash

# Philosophers Visualizer - Shell script version
# Usage: ./philo_visualizer.sh [path_to_philo_executable] [args...]

# Colors and symbols
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Symbols for each state
THINKING="🤔"
FORK="🍴"
EATING="🍝"
SLEEPING="💤"
DEAD="💀"
TABLE="◯"

# Check arguments
if [ $# -lt 1 ]; then
    echo "Usage: $0 [path_to_philo_executable] [args...]"
    exit 1
fi

PHILO_PATH="$1"
shift

# Check if executable exists
if [ ! -x "$PHILO_PATH" ]; then
    echo "Error: $PHILO_PATH is not an executable file"
    exit 1
fi

# Parse number of philosophers
if [ $# -lt 1 ]; then
    echo "Error: Please provide number of philosophers"
    exit 1
fi

NUM_PHILOS=$1
TIME_TO_DIE=$2
TIME_TO_EAT=$3
TIME_TO_SLEEP=$4
MUST_EAT=${5:-"-"}

# Data structures
declare -A philo_states
declare -A philo_eat_counts
declare -A philo_last_eat_times

# Initialize philosophers
for (( i=1; i<=$NUM_PHILOS; i++ )); do
    philo_states[$i]="thinking"
    philo_eat_counts[$i]=0
    philo_last_eat_times[$i]=0
done

# Start time
START_TIME=$(date +%s%3N)

# Function to clear screen and position cursor
function clear_screen {
    clear
    echo -e "${BOLD}Philosophers Visualizer${NC}"
    echo "N=$NUM_PHILOS Die=$TIME_TO_DIE Eat=$TIME_TO_EAT Sleep=$TIME_TO_SLEEP MustEat=$MUST_EAT"
    echo ""
}

# Function to draw table
function draw_table {
    clear_screen
    
    # Calculate elapsed time
    CURRENT_TIME=$(date +%s%3N)
    ELAPSED=$((CURRENT_TIME - START_TIME))
    echo "Elapsed time: ${ELAPSED}ms"
    echo ""
    
    # Draw philosophers in a circle-like format
    local spacing=$((70 / (NUM_PHILOS + 1)))
    local table_line=""
    local philo_line=""
    local status_line=""
    
    # Create table line with forks
    for (( i=1; i<=$NUM_PHILOS; i++ )); do
        table_line+=$(printf "%${spacing}s" "$TABLE")
    done
    
    # Create philosopher line
    for (( i=1; i<=$NUM_PHILOS; i++ )); do
        # Get state and color
        state=${philo_states[$i]}
        case $state in
            "thinking")
                COLOR=$BLUE
                SYMBOL=$THINKING
                ;;
            "has taken a fork")
                COLOR=$YELLOW
                SYMBOL=$FORK
                ;;
            "is eating")
                COLOR=$GREEN
                SYMBOL=$EATING
                ;;
            "is sleeping")
                COLOR=$MAGENTA
                SYMBOL=$SLEEPING
                ;;
            "died")
                COLOR=$RED
                SYMBOL=$DEAD
                ;;
            *)
                COLOR=$NC
                SYMBOL="?"
                ;;
        esac
        
        philo_line+=$(printf "%${spacing}s" "${COLOR}${SYMBOL}${NC}")
    done
    
    # Create status line with eat counts
    for (( i=1; i<=$NUM_PHILOS; i++ )); do
        status_line+=$(printf "%${spacing}s" "${i}:${philo_eat_counts[$i]}")
    done
    
    # Print visualization
    echo -e "Table:  $table_line"
    echo -e "Philos: $philo_line"
    echo -e "ID:Eat  $status_line"
    echo ""
    echo "Legend:"
    echo -e "${BLUE}${THINKING}${NC} Thinking | ${YELLOW}${FORK}${NC} Has Fork | ${GREEN}${EATING}${NC} Eating | ${MAGENTA}${SLEEPING}${NC} Sleeping | ${RED}${DEAD}${NC} Died"
    echo ""
    echo "Recent logs:"
}

# Function to parse philo output
function parse_output {
    local line="$1"
    
    # Parse timestamp, philo_id, and action
    if [[ $line =~ ([0-9]+)\ ([0-9]+)\ (.+) ]]; then
        local timestamp="${BASH_REMATCH[1]}"
        local philo_id="${BASH_REMATCH[2]}"
        local action="${BASH_REMATCH[3]}"
        
        # Update philosopher state
        philo_states[$philo_id]="$action"
        
        # Update eat count if eating
        if [ "$action" == "is eating" ]; then
            philo_eat_counts[$philo_id]=$((philo_eat_counts[$philo_id] + 1))
            philo_last_eat_times[$philo_id]=$timestamp
        fi
    fi
}

# Run the philosophers program and visualize
draw_table
echo "Starting philosophers simulation..."

# Create a temporary file for philo output
TEMP_FILE=$(mktemp)
trap "rm -f $TEMP_FILE; kill 0" EXIT INT TERM

# Run philo in the background, redirecting to temp file
"$PHILO_PATH" "$@" > "$TEMP_FILE" &
PHILO_PID=$!

# Tail the output and update visualization
tail -f "$TEMP_FILE" | while read -r line; do
    parse_output "$line"
    draw_table
    echo "$line"
done

wait $PHILO_PID
