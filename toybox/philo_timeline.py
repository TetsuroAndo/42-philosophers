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
from collections import deque, defaultdict

# ANSI colors for terminal output (fallback if curses is not used)
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

# Philosopher states with their colors and symbols
STATES = {
    "thinking": {"color": curses.COLOR_BLUE, "symbol": "T", "label": "Thinking", "ansi": COLORS["BLUE"]},
    "has taken a fork": {"color": curses.COLOR_YELLOW, "symbol": "F", "label": "Fork", "ansi": COLORS["YELLOW"]},
    "is eating": {"color": curses.COLOR_GREEN, "symbol": "E", "label": "Eating", "ansi": COLORS["GREEN"]},
    "is sleeping": {"color": curses.COLOR_MAGENTA, "symbol": "S", "label": "Sleeping", "ansi": COLORS["MAGENTA"]},
    "died": {"color": curses.COLOR_RED, "symbol": "D", "label": "Died", "ansi": COLORS["RED"]},
}

class Event:
    def __init__(self, timestamp, philo_id, state):
        self.timestamp = timestamp
        self.philo_id = philo_id
        self.state = state

class TimelineVisualizer:
    def __init__(self, philo_path, args):
        self.philo_path = philo_path
        self.args = args
        self.process = None
        self.running = True
        self.log_buffer = deque(maxlen=15)  # Last 15 log lines
        self.start_time = None
        self.events = []  # List of all events
        self.current_states = {}  # Current state of each philosopher
        self.timeline_data = defaultdict(list)  # Timeline data for each philosopher
        self.max_timestamp = 0  # Maximum timestamp seen so far
        self.scroll_position = 0  # Horizontal scroll position
        self.time_scale = 1.0  # Time scale: 1.0 = 1 pixel per ms
        
        # Parse args to get number of philosophers
        self.num_philos = int(args[0]) if args else 0
        self.time_to_die = int(args[1]) if len(args) > 1 else 0
        self.time_to_eat = int(args[2]) if len(args) > 2 else 0
        self.time_to_sleep = int(args[3]) if len(args) > 3 else 0
        self.must_eat = int(args[4]) if len(args) > 4 else -1
        
        # Initialize philosopher states
        for i in range(1, self.num_philos + 1):
            self.current_states[i] = "thinking"
    
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
            
            # Record the event
            event = Event(timestamp, philo_id, action)
            self.events.append(event)
            
            # Update current states
            self.current_states[philo_id] = action
            
            # Add to timeline data - each item is (start_time, end_time, state)
            if self.timeline_data[philo_id]:
                # Update end time of previous state
                last_entry = self.timeline_data[philo_id][-1]
                last_entry[1] = timestamp  # Set end time to current timestamp
            
            # Add new state (start time, end time=None, state)
            self.timeline_data[philo_id].append([timestamp, None, action])
            
            # Update max timestamp
            self.max_timestamp = max(self.max_timestamp, timestamp)

    def draw_timeline(self, stdscr):
        """Draw the timeline visualization using curses"""
        height, width = stdscr.getmaxyx()
        
        # Initialize colors if not already done
        if not hasattr(self, 'colors_initialized'):
            curses.start_color()
            curses.use_default_colors()
            
            # Initialize color pairs
            curses.init_pair(1, curses.COLOR_BLUE, -1)     # Thinking
            curses.init_pair(2, curses.COLOR_YELLOW, -1)   # Has fork
            curses.init_pair(3, curses.COLOR_GREEN, -1)    # Eating
            curses.init_pair(4, curses.COLOR_MAGENTA, -1)  # Sleeping
            curses.init_pair(5, curses.COLOR_RED, -1)      # Died
            curses.init_pair(6, curses.COLOR_WHITE, -1)    # Default
            
            self.colors_initialized = True
        
        # Clear screen
        stdscr.clear()
        
        # Draw title and parameters
        title = "Philosophers Timeline Visualizer"
        stdscr.addstr(0, (width - len(title)) // 2, title, curses.A_BOLD)
        
        params = f"N={self.num_philos} Die={self.time_to_die}ms Eat={self.time_to_eat}ms Sleep={self.time_to_sleep}ms"
        if self.must_eat > 0:
            params += f" MustEat={self.must_eat}"
        stdscr.addstr(1, (width - len(params)) // 2, params)
        
        # Display time scale and controls
        controls = "Controls: [←/→] Scroll, [+/-] Zoom, [r] Reset view, [q] Quit"
        scale_info = f"Scale: {self.time_scale:.2f}x"
        stdscr.addstr(2, 2, scale_info)
        stdscr.addstr(2, width - len(controls) - 2, controls)
        
        # Draw timeline legend
        legend_y = 3
        legend_x = 2
        stdscr.addstr(legend_y, legend_x, "Legend: ")
        legend_x += 8
        
        for state, info in STATES.items():
            state_str = f"{info['symbol']}={info['label']}"
            stdscr.addstr(legend_y, legend_x, state_str, curses.color_pair(self._get_color_pair(state)))
            legend_x += len(state_str) + 3
        
        # Calculate timeline area
        timeline_start_y = 5
        timeline_height = min(self.num_philos * 2 + 1, height - timeline_start_y - 15)
        timeline_width = width - 15  # Leave space for labels
        
        # Draw timeline header (time marks)
        header_y = timeline_start_y - 1
        label_x = 2
        timeline_x = 15
        
        stdscr.addstr(header_y, label_x, "Philo")
        
        # Draw time markers
        visible_time_range = int(timeline_width / self.time_scale)
        time_start = self.scroll_position
        time_end = time_start + visible_time_range
        
        # Draw tick marks every 100ms
        tick_interval = max(100, int(100 / self.time_scale))
        for t in range((time_start // tick_interval) * tick_interval, time_end, tick_interval):
            if t < 0:
                continue
                
            x_pos = timeline_x + int((t - time_start) * self.time_scale)
            if x_pos < timeline_x or x_pos >= timeline_x + timeline_width:
                continue
                
            # Draw vertical tick mark
            stdscr.addstr(header_y, x_pos, "┬")
            
            # Draw time label every 500ms
            if t % 500 == 0:
                time_label = str(t)
                if x_pos + len(time_label) // 2 < width:
                    stdscr.addstr(header_y - 1, x_pos - len(time_label) // 2, time_label)
        
        # Draw horizontal timeline axis
        for x in range(timeline_x, timeline_x + timeline_width):
            stdscr.addstr(header_y, x, "─")
        
        # Draw philosopher timelines
        for i in range(1, self.num_philos + 1):
            y_pos = timeline_start_y + (i - 1) * 2
            
            # Draw philosopher label
            philo_label = f"Philo {i}"
            stdscr.addstr(y_pos, label_x, philo_label)
            
            # Draw timeline for this philosopher
            if i in self.timeline_data:
                for start, end, state in self.timeline_data[i]:
                    if end is None:
                        end = self.max_timestamp  # Use current max time for ongoing states
                    
                    # Calculate pixel positions
                    start_x = timeline_x + int((start - time_start) * self.time_scale)
                    end_x = timeline_x + int((end - time_start) * self.time_scale)
                    
                    # Skip if completely out of view
                    if end_x < timeline_x or start_x >= timeline_x + timeline_width:
                        continue
                    
                    # Clip to visible area
                    start_x = max(start_x, timeline_x)
                    end_x = min(end_x, timeline_x + timeline_width)
                    
                    # Draw the timeline segment with the appropriate color
                    color_pair = self._get_color_pair(state)
                    for x in range(start_x, end_x):
                        try:
                            stdscr.addstr(y_pos, x, STATES[state]["symbol"], curses.color_pair(color_pair))
                        except curses.error:
                            # Ignore errors from writing at the bottom-right corner
                            pass
        
        # Draw current time indicator
        if self.start_time:
            current_elapsed = int(time.time() * 1000 - self.start_time)
            current_x = timeline_x + int((current_elapsed - time_start) * self.time_scale)
            
            if current_x >= timeline_x and current_x < timeline_x + timeline_width:
                for y in range(timeline_start_y, timeline_start_y + timeline_height):
                    stdscr.addstr(y, current_x, "│", curses.A_BOLD)
        
        # Draw recent logs at the bottom
        log_y = timeline_start_y + timeline_height + 1
        stdscr.addstr(log_y, 2, "Recent logs:", curses.A_BOLD)
        for i, log in enumerate(self.log_buffer):
            if log_y + i + 1 < height - 1:
                stdscr.addstr(log_y + i + 1, 2, log[:width-4])
        
        # Draw elapsed time
        elapsed = int(time.time() * 1000 - self.start_time) if self.start_time else 0
        time_str = f"Elapsed time: {elapsed}ms"
        stdscr.addstr(height-1, 2, time_str)
        
        # Refresh screen
        stdscr.refresh()

    def _get_color_pair(self, state):
        """Get color pair ID for a given state"""
        if state == "thinking":
            return 1
        elif state == "has taken a fork":
            return 2
        elif state == "is eating":
            return 3
        elif state == "is sleeping":
            return 4
        elif state == "died":
            return 5
        else:
            return 6  # Default

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
                elif key == curses.KEY_LEFT:
                    # Scroll left
                    self.scroll_position = max(0, self.scroll_position - int(100 * self.time_scale))
                elif key == curses.KEY_RIGHT:
                    # Scroll right
                    self.scroll_position += int(100 * self.time_scale)
                elif key == ord('+') or key == ord('='):
                    # Zoom in (higher scale = more pixels per ms)
                    self.time_scale = min(10.0, self.time_scale * 1.2)
                elif key == ord('-'):
                    # Zoom out (lower scale = fewer pixels per ms)
                    self.time_scale = max(0.1, self.time_scale / 1.2)
                elif key == ord('r'):
                    # Reset view
                    self.scroll_position = 0
                    self.time_scale = 1.0
            except KeyboardInterrupt:
                self.running = False
                if self.process:
                    self.process.terminate()
                break
            
            # Draw the timeline
            self.draw_timeline(stdscr)
            
            # Auto-scroll if following current time
            if self.start_time and (key != curses.KEY_LEFT and key != curses.KEY_RIGHT):
                current_elapsed = int(time.time() * 1000 - self.start_time)
                height, width = stdscr.getmaxyx()
                timeline_width = width - 15
                visible_range = int(timeline_width / self.time_scale)
                
                # Auto-scroll if current time is not in the middle third of the view
                if current_elapsed > self.scroll_position + (visible_range * 2 // 3):
                    self.scroll_position = current_elapsed - (visible_range // 2)
            
            # Sleep to limit refresh rate
            time.sleep(0.1)

def main():
    parser = argparse.ArgumentParser(description='Philosopher Timeline Visualizer')
    parser.add_argument('philo_path', help='Path to philo executable')
    parser.add_argument('args', nargs='*', help='Arguments for philo program')
    
    args = parser.parse_args()
    
    if not os.path.isfile(args.philo_path) or not os.access(args.philo_path, os.X_OK):
        print(f"Error: {args.philo_path} is not an executable file")
        return 1
    
    # Run the visualizer
    visualizer = TimelineVisualizer(args.philo_path, args.args)
    
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