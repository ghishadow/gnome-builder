#!/usr/bin/env python3

#
# html_preview_plugin.py
#
# Copyright (C) 2015 Christian Hergert <chris@dronelabs.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import gi
import os

gi.require_version('Gtk', '3.0')
gi.require_version('Ide', '1.0')
gi.require_version('WebKit2', '4.0')

from gi.repository import GLib
from gi.repository import Gio
from gi.repository import Gtk
from gi.repository import GObject
from gi.repository import Ide
from gi.repository import WebKit2
from gi.repository import Peas

_ = Ide.gettext

class HtmlPreviewData(GObject.Object, Ide.ApplicationAddin):
    MARKDOWN_CSS = None
    MARKED_JS = None
    MARKDOWN_VIEW_JS = None

    def do_load(self, app):
        HtmlPreviewData.MARKDOWN_CSS = self.get_data('markdown.css')
        HtmlPreviewData.MARKED_JS = self.get_data('marked.js')
        HtmlPreviewData.MARKDOWN_VIEW_JS = self.get_data('markdown-view.js')

    def get_data(self, name):
        engine = Peas.Engine.get_default()
        info = engine.get_plugin_info('html_preview_plugin')
        datadir = info.get_data_dir()
        path = os.path.join(datadir, name)
        return open(path, 'r').read()


class HtmlPreviewAddin(GObject.Object, Ide.EditorViewAddin):
    def do_load(self, editor):
        self.action = Gio.SimpleAction(name='preview-as-html', enabled=True)
        self.action.connect('activate', lambda *_: self.preview_activated(editor))

        actions = editor.get_action_group('view')
        actions.add_action(self.action)

    def do_unload(self, editor):
        actions = editor.get_action_group('view')
        actions.remove_action('preview-as-html')

    def do_language_changed(self, language_id):
        enabled = (language_id in ('html', 'markdown'))
        self.action.set_enabled(enabled)

    def preview_activated(self, editor):
        document = editor.get_document()
        view = HtmlPreviewView(document, visible=True)
        stack = editor.get_ancestor(Ide.LayoutStack)
        stack.add(view)

class HtmlPreviewView(Ide.LayoutView):
    markdown = False

    def __init__(self, document, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.document = document

        self.webview = WebKit2.WebView(visible=True, expand=True)
        self.add(self.webview)

        settings = self.webview.get_settings()
        settings.enable_html5_local_storage = False

        language = document.get_language()
        if language and language.get_id() == 'markdown':
            self.markdown = True

        document.connect('changed', self.on_changed)
        self.on_changed(document)

    def do_get_title(self):
        title = self.document.get_title()
        return '%s (Preview)' % title

    def do_get_document(self):
        return self.document

    def get_markdown(self, text):
        text = text.replace("\"", "\\\"").replace("\n", "\\n")
        params = (HtmlPreviewData.MARKDOWN_CSS,
                  text,
                  HtmlPreviewData.MARKED_JS,
                  HtmlPreviewData.MARKDOWN_VIEW_JS)

        return """
<html>
 <head>
  <style>%s</style>
  <script>var str="%s";</script>
  <script>%s</script>
  <script>%s</script>
 </head>
 <body onload="preview()">
  <div class="markdown-body" id="preview">
  </div>
 </body>
</html>
""" % params

    def reload(self):
        base_uri = self.document.get_file().get_file().get_uri()

        begin, end = self.document.get_bounds()
        text = self.document.get_text(begin, end, True)

        if self.markdown:
            text = self.get_markdown(text)

        self.webview.load_html(text, base_uri)

    def on_changed(self, document):
        self.reload()
