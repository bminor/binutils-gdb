static unsigned
hw_com_io_write_buffer_callback(device *me,
				 const void *source,
				 int space,
				 unsigned_word addr,
				 unsigned nr_bytes,
				 cpu *processor,
				 unsigned_word cia)
{
  hw_com_device *hw_com = (hw_com_device*)device_data(me);
  unsigned_1 val = *(unsigned_1*)source;

  switch ((int)addr & hw_com_offset_mask) {

  default:
    error("hw_com_write_callback() internal error\n");

  }
	 
  return nr_bytes;
}

#endif /* _HW_COM_ */
