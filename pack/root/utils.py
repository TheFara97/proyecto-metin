class Sandbox(object):
	WHITE_LIST = ['__builtin__', 'types', __name__, '__main__', 'sys']
	def __init__(self, prevent_imported_modules = False, allowed_modules = [], prevented_modules = [], allowed_paths = []):
		self.prevent_imported_modules = prevent_imported_modules
		self.allowed_modules = allowed_modules
		self.prevented_modules = prevented_modules
		self.allowed_paths = allowed_paths

	def add_allowed_modules(self, allowed_modules):
		self.allowed_modules = self.allowed_modules + allowed_modules

	def add_prevented_modules(self, prevented_modules):
		self.prevented_modules = self.prevented_modules + prevented_modules

	def execfile(self, filename, dic):
		import sys
		import copy

		for allowed_module_name in self.allowed_modules:
			try:
				exec 'import {0}'.format(allowed_module_name)
			except:
				sys.modules['dbg'].TraceError("UISCRIPT_LOAD_ERROR: Could not import %s [filename %s]" % (allowed_module_name, filename))
				pass
		old_modules = copy.copy(sys.modules)

		old_path = sys.path
		sys.path = self.allowed_paths

		if self.prevent_imported_modules:
			import types
			for k, v in old_modules.items():
				if type(v) == types.ModuleType:
					if not (k in self.WHITE_LIST or k in self.allowed_modules):
						sys.modules[k] = None

		for prevented_module_name in self.prevented_modules:
			sys.modules[prevented_module_name] = None

		try:
			f = open(filename, 'rb')
			data = f.read()
			code = compile(data, filename, 'exec')
			exec code in dic
		except Exception, e:
			old_modules['dbg'].TraceError("UISCRIPT_LOAD_ERROR: %s [filename %s]" % (str(e), filename))
		finally:
			for k, v in sys.modules.items():
				if not k in old_modules:
					del sys.modules[k]
			for k, v in old_modules.items():
				sys.modules[k] = v

			sys.path = old_path



def GetElementDictByName(dct, name):
	if 'children' in dct:
		for child in dct['children']:
			if 'name' in child:
				if child['name'] == name:
					return child

			if 'children' in child:
				search = GetElementDictByName(child, name)
				if search != None:
					return search
	return None






