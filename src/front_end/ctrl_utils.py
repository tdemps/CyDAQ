class utils:
    def val_str_as_int(val, min, max):
        """
        Checks if a value, as a string, is within the range set by the
        parameters min and max.

        Args:
            val: String to be checked
            min: Minimum int value
            max: Maximum int value

        Returns:
            True if value is within the set range
        """
        if(val == ""):
            ret = False
        else:
            temp_int = int(val)

            if(temp_int >= min and temp_int <= max):
                ret = True
            else:
                ret = False

        return ret
