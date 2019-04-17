# howdy/urls.py
from django.conf.urls import url
from howdy import views
from django.urls import path

urlpatterns = [
  path(r'^$', views.HomePageView.as_view()),
]
