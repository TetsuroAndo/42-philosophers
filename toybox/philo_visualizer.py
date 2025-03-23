#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import subprocess
import sys
import time
import threading
import re
import os
import signal
import argparse
import curses
from collections import deque

# ANSI colors
COLORS = {
    "RESET": "\033[0m",
    "BOLD": "\033[1m",
    "RED": "\033[31m",
    "GREEN": "\033[32m",
    "YELLOW": "\033[33m",
    "BLUE": "\033[34m",
    "MAGENTA": "\033[35m",
    "CYAN": "\033[36m",
    "WHITE": "\033[37m",
}

# Philosopher states and their colors
STATES = {
    "thinking": (COLORS["BLUE"], "🤔"),
    "has taken a fork": (COLORS["YELLOW"], "🍴"),
    "is eating": (COLORS["GREEN"], "🍝"),
    "is sleeping": (COLORS["MAGENTA"], "💤"),
    "died": (COLORS["RED"], "💀"),
}

class PhilosopherVisualizer:
    def __init__(self, philo_path, args):
        self.philo_path = philo_path
        self.args = args
        self.philosophers = {}
        self.forks = {}
        self.process = None
        self.running = True
        self.log_buffer = deque(maxlen=10)  # Last 10 log lines
        self.start_time = None
        
        # Parse args to get number of philosophers
        self.num_philos = int(args[0]) if args else 0
        self.time_to_die = int(args[1]) if len(args) > 1 else 0
        self.time_to_eat = int(args[2]) if len(args) > 2 else 0
        self.time_to_sleep = int(args[3]) if len(args) > 3 else 0
        self.must_eat = int(args[4]) if len(args) > 4 else -1
        
        # Initialize forks (all available at start)
        for i in range(1, self.num_philos + 1):
            self.forks[i] = True  # True means fork is available
        
        # Initialize philosophers
        for i in range(1, self.num_philos + 1):
            self.philosophers[i] = {
                "state": "thinking",
                "last_eat_time": 0,
                "eat_count": 0,
                "left_fork": i,
                "right_fork": (i % self.num_philos) + 1,
                "has_left_fork": False,
                "has_right_fork": False,
            }

    def run_philo(self):
        """Run the philosophers program and capture its output"""
        cmd = [self.philo_path] + self.args
        self.process = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1
        )
        self.start_time = time.time() * 1000  # Current time in ms
        
        # Read output line by line
        for line in iter(self.process.stdout.readline, ""):
            if not self.running:
                break
            
            self.log_buffer.append(line.strip())
            self.parse_output(line)

    def parse_output(self, line):
        """Parse a line of output from the philo program"""
        # Example line: "200 3 is eating"
        pattern = r"(\d+) (\d+) (.+)"
        match = re.match(pattern, line)
        
        if match:
            timestamp = int(match.group(1))
            philo_id = int(match.group(2))
            action = match.group(3)
            
            if philo_id in self.philosophers:
                # Update philosopher state
                self.philosophers[philo_id]["state"] = action
                
                # Update fork states based on actions
                if action == "has taken a fork":
                    # Determine which fork was taken (not perfect but an estimation)
                    left_id = self.philosophers[philo_id]["left_fork"]
                    right_id = self.philosophers[philo_id]["right_fork"]
                    
                    if not self.philosophers[philo_id]["has_left_fork"] and self.forks[left_id]:
                        self.philosophers[philo_id]["has_left_fork"] = True
                        self.forks[left_id] = False
                    elif not self.philosophers[philo_id]["has_right_fork"] and self.forks[right_id]:
                        self.philosophers[philo_id]["has_right_fork"] = True
                        self.forks[right_id] = False
                
                elif action == "is eating":
                    self.philosophers[philo_id]["last_eat_time"] = timestamp
                    self.philosophers[philo_id]["eat_count"] += 1
                
                elif action == "is sleeping" or action == "is thinking":
                    # Release forks
                    if self.philosophers[philo_id]["has_left_fork"]:
                        self.forks[self.philosophers[philo_id]["left_fork"]] = True
                        self.philosophers[philo_id]["has_left_fork"] = False
                    if self.philosophers[philo_id]["has_right_fork"]:
                        self.forks[self.philosophers[philo_id]["right_fork"]] = True
                        self.philosophers[philo_id]["has_right_fork"] = False

    def draw_table(self, stdscr):
        """Draw the table with philosophers in curses"""
        height, width = stdscr.getmaxyx()
        
        # Clear screen
        stdscr.clear()
        
        # Determine table radius based on terminal size
        table_radius = min(height // 3, width // 4)
        center_y, center_x = height // 2, width // 2
        
        # Draw title
        title = "Philosophers Visualizer"
        stdscr.addstr(1, (width - len(title)) // 2, title, curses.A_BOLD)
        
        # Draw simulation parameters
        params = f"N={self.num_philos} Die={self.time_to_die}ms Eat={self.time_to_eat}ms Sleep={self.time_to_sleep}ms"
        if self.must_eat > 0:
            params += f" MustEat={self.must_eat}"
        stdscr.addstr(2, (width - len(params)) // 2, params)
        
        # Draw elapsed time
        elapsed = int(time.time() * 1000 - self.start_time) if self.start_time else 0
        time_str = f"Elapsed time: {elapsed}ms"
        stdscr.addstr(3, (width - len(time_str)) // 2, time_str)
        
        # Draw philosophers around the table
        angle_step = 2 * 3.14159 / self.num_philos
        import math
        
        for i in range(1, self.num_philos + 1):
            angle = (i - 1) * angle_step
            x = int(center_x + table_radius * math.cos(angle))
            y = int(center_y + table_radius * math.sin(angle))
            
            philo = self.philosophers[i]
            state = philo["state"]
            color, emoji = STATES.get(state, (COLORS["WHITE"], "?"))
            
            # Draw philosopher ID
            stdscr.addstr(y-1, x-1, f"{i}")
            
            # Draw philosopher state emoji
            stdscr.addstr(y, x, emoji)
            
            # Draw eating count
            eat_count = philo["eat_count"]
            stdscr.addstr(y+1, x-1, f"E:{eat_count}")
        
        # Draw table
        for i in range(360):
            angle = i * 3.14159 / 180
            r = table_radius // 2
            x = int(center_x + r * math.cos(angle))
            y = int(center_y + r * math.sin(angle))
            stdscr.addstr(y, x, "·")
        
        # Draw forks around the table
        for i in range(1, self.num_philos + 1):
            angle = (i - 0.5) * angle_step
            r = (table_radius + table_radius // 2) // 2
            x = int(center_x + r * math.cos(angle))
            y = int(center_y + r * math.sin(angle))
            
            fork_char = "🍴" if self.forks[i] else " "
            stdscr.addstr(y, x, fork_char)
        
        # Draw recent logs at the bottom
        log_y = height - len(self.log_buffer) - 2
        stdscr.addstr(log_y - 1, 2, "Recent logs:", curses.A_BOLD)
        for i, log in enumerate(self.log_buffer):
            if log_y + i < height - 1:
                stdscr.addstr(log_y + i, 2, log[:width-4])
        
        # Draw help text
        help_text = "Press 'q' to quit"
        stdscr.addstr(height-1, (width - len(help_text)) // 2, help_text)
        
        # Refresh screen
        stdscr.refresh()

    def curses_main(self, stdscr):
        """Main curses loop"""
        # Configure curses
        curses.curs_set(0)  # Hide cursor
        stdscr.timeout(100)  # Non-blocking input with 100ms timeout
        
        # Start philo program in a thread
        philo_thread = threading.Thread(target=self.run_philo)
        philo_thread.daemon = True
        philo_thread.start()
        
        # Main visualization loop
        while self.running:
            # Handle key presses
            try:
                key = stdscr.getch()
                if key == ord('q'):
                    self.running = False
                    if self.process:
                        self.process.terminate()
                    break
            except KeyboardInterrupt:
                self.running = False
                if self.process:
                    self.process.terminate()
                break
            
            # Draw the table
            self.draw_table(stdscr)
            
            # Sleep to limit refresh rate
            time.sleep(0.1)

def main():
    parser = argparse.ArgumentParser(description='Philosopher Visualizer')
    parser.add_argument('philo_path', help='Path to philo executable')
    parser.add_argument('args', nargs='*', help='Arguments for philo program')
    
    args = parser.parse_args()
    
    if not os.path.isfile(args.philo_path) or not os.access(args.philo_path, os.X_OK):
        print(f"Error: {args.philo_path} is not an executable file")
        return 1
    
    # Run the visualizer
    visualizer = PhilosopherVisualizer(args.philo_path, args.args)
    
    try:
        curses.wrapper(visualizer.curses_main)
    except KeyboardInterrupt:
        pass
    finally:
        # Clean up
        if visualizer.process:
            try:
                visualizer.process.terminate()
                visualizer.process.wait(timeout=1)
            except:
                visualizer.process.kill()
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
