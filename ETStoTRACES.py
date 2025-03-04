import struct
import estraces

def convert_ets_to_traces(ets_file_path, output_traces_path):
    """
    Convert ETS file to TRACES file.
    
    :param ets_file_path: Path to the .ets file
    :param output_traces_path: Path to save the .traces file
    """
    # Read .ets file using estraces
    ths = estraces.read_ths_from_ets_file(ets_file_path)
    
    # Get trace count and trace length
    samples = ths.samples
    trace_count = len(samples)  # Number of traces
    trace_length = len(samples[0])  # Length of each trace

    print(f"Trace Count: {trace_count}, Trace Length: {trace_length}")

    # Open .traces file for writing
    with open(output_traces_path, 'wb') as traces_file:
        # Write the header
        traces_file.write(struct.pack('i', trace_length))  # Write trace length
        traces_file.write(struct.pack('i', trace_count))  # Write trace count
        
        # Write each trace
        for index, trace_data in enumerate(samples):
            traces_file.write(struct.pack(f'{trace_length}f', *trace_data))  # Write trace as floats
            
            if (index + 1) % 100 == 0:
                print(f"Processed {index + 1}/{trace_count} traces")

    print(f"Converted {trace_count} traces to {output_traces_path}")

# Paths
ets_file_path = "C:\\Users\\lhs\\Desktop\\subchannel\\trc.ets"
output_traces_path = "C:\\Users\\lhs\\Desktop\\subchannel\\AES.traces"

# Convert the ETS file to TRACES file
convert_ets_to_traces(ets_file_path, output_traces_path)