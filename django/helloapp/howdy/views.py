# howdy/views.py

from django.shortcuts import render
from django.views.generic import TemplateView
import logging
import howdy.models

# Create your views here.
class HomePageView(TemplateView):
  def get(self, request, **kwargs):
    temperature = howdy.models.get_current_temperature ()
    return render(request, 'index.html', context = {'temp': temperature})

class AboutPageView(TemplateView):
  template_name = "about.html"

