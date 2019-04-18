from django.conf.urls import url
from howdy import views
# from django.urls import path

urlpatterns = [
  url(r'^about/$', views.AboutPageView.as_view()),
  url(r'^$', views.HomePageView.as_view()),
]
