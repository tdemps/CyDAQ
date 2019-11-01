import jsonpickle

from tkinter import filedialog

from mdl_global import mdl_const
from mdl_firmware import mdl_fw_vals


class ctrl_settings:
    """
    This class defines control logic for saving and loading settings.

    @author: sdmay18-31
    """

    def promnt_save():
        """
        Callback for saving current firmware configuration as a JSON
        file.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        f_ptr = filedialog.asksaveasfile(initialdir=cnst.fout_dflt_dir,
                                         title="Save settings file: ",
                                         mode='w',
                                         defaultextension=".json",
                                         filetypes=[("JSON (*.json)",
                                                     "*.json")])
        if(f_ptr is not None):
            cfg = mdl_fw_vals()
            jsonpickle.set_encoder_options('json',
                                           sort_keys=True,
                                           indent=4)
            f_ptr.write(jsonpickle.encode(cfg))
            f_ptr.close()

    def promnt_load():
        """
        Callback for loading a firmware configuration from a JSON file.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        f_ptr = filedialog.askopenfile(initialdir=cnst.fout_dflt_dir,
                                       title="Open settings file: ",
                                       mode='r',
                                       filetypes=[("JSON (*.json)",
                                                   "*.json")])
        if(f_ptr is not None):
            cfg = mdl_fw_vals()
            obj = jsonpickle.decode(f_ptr.read())
            f_ptr.close()
            cfg.copy(obj)
            cnst.top_wndw.left_frameFrame.update_text()
